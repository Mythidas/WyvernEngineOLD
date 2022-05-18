#include "wvpch.h"
#include "Renderer2D.h"

#include "Framebuffer.h"
#include "VertexArray.h"

#include <Wyvern/Core/AssetManager.h>
#include <Wyvern/Core/Math/Matrix4x4.h>
#include <Wyvern/Core/Application/Application.h>

#include <glad.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Wyvern::Renderer
{
	Ref<Framebuffer> Renderer2D::s_Framebuffer;

	static const size_t MaxQuadCount = 1000;
	static const size_t MaxVertexCount = MaxQuadCount * 4;
	static const size_t MaxIndexCount = MaxQuadCount * 6;
	static const size_t MaxTextures = 32;

	struct RenderData
	{
		GLuint VAO = 0;
		GLuint VBO = 0;
		GLuint IBO = 0;

		GLuint SVAO = 0;
		GLuint SVBO = 0;

		Vertex* QuadBuffer = nullptr;
		Vertex* QuadBufferPtr = nullptr;
		std::multimap<float, Ref<VertexArray>> VertexData;

		uint32_t IndexCount = 0;

		std::array<uint32_t, MaxTextures> TextureSlots;
		uint32_t TextureSlotIndex = 1;
		uint32_t WhiteTexture = 0;

		CameraRenderer* Camera = nullptr;
		Transform* CameraPosition = nullptr;
	};

	struct Character
	{
		uint32_t TextureID = 0;
		Vector2 Size;
		Vector2 Bearing;
		uint32_t Advance = 0;
	};

	static std::unordered_map<GLchar, Character> Characters;
	static RenderData s_Data;

	void Renderer2D::OnAttach()
	{
		// Framebuffer

		FramebufferSpecification fbSpec;
		fbSpec.Attachments =
		{
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::Depth
		};
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		s_Framebuffer = CreateRef<Framebuffer>(fbSpec);

		// Quad Renderer

		s_Data.QuadBuffer = new Vertex[MaxVertexCount];

		glGenVertexArrays(1, &s_Data.VAO);
		glBindVertexArray(s_Data.VAO);

		glGenBuffers(1, &s_Data.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VBO);
		glBufferData(GL_ARRAY_BUFFER, MaxVertexCount *  sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexArrayAttrib(s_Data.VAO, 0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

		glEnableVertexArrayAttrib(s_Data.VAO, 1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

		glEnableVertexArrayAttrib(s_Data.VAO, 2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));

		glEnableVertexArrayAttrib(s_Data.VAO, 3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

		glEnableVertexArrayAttrib(s_Data.VAO, 4);
		glVertexAttribIPointer(4, 1, GL_INT, sizeof(Vertex), (const void*)offsetof(Vertex, EntityID));

		uint32_t indices[MaxIndexCount];
		uint32_t offset = 0;
		for (size_t i = 0; i < MaxIndexCount; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		glGenBuffers(1, &s_Data.IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Screen Renderer

		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &s_Data.SVAO);
		glGenBuffers(1, &s_Data.SVBO);
		glBindVertexArray(s_Data.SVAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.SVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Setup default texture

		glCreateTextures(GL_TEXTURE_2D, 1, &s_Data.WhiteTexture);
		glBindTexture(GL_TEXTURE_2D, s_Data.WhiteTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		uint32_t color = 0xffffffff;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
		for (size_t i = 1; i < MaxTextures; i++)
			s_Data.TextureSlots[i] = 0;

		// FreeType Renderer

		FT_Library ft;

		if (FT_Init_FreeType(&ft))
		{
			DEBUG_LOG_ERROR("Error: FreeType could not init Library");
		}

		std::string resources = Application::GetResourcesPath();
		FT_Face face;
		if (FT_New_Face(ft, (resources + "\\Fonts\\Times.TTF").c_str(), 0, &face))
		{
			DEBUG_LOG_ERROR("Error: FreeType failed to load font");
		}
		else
		{
			FT_Set_Pixel_Sizes(face, 0, 48);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			for (unsigned char c = 0; c < 128; c++)
			{
				if (FT_Load_Char(face, c, 1L << 2))
				{
					DEBUG_LOG_ERROR("Error: FreeType Failed to load Glyph");
				}

				uint32_t texture;
				
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				Character character =
				{
					texture,
					Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<uint32_t>(face->glyph->advance.x)
				};

				Characters.insert(std::pair<char, Character>(c, character));
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	void Renderer2D::OnDestroy()
	{
		glDeleteVertexArrays(1, &s_Data.VAO);
		glDeleteBuffers(1, &s_Data.VBO);
		glDeleteBuffers(1, &s_Data.IBO);

		delete[] s_Data.QuadBuffer;
	}
	void Renderer2D::BeginScene(CameraRenderer* cameraRenderer, Transform* cameraPosition)
	{
		s_Framebuffer->Invalidate();
		s_Framebuffer->Bind();
		s_Framebuffer->ClearColorAttachment(1, -1);
		AssetManager::GetShader("ScreenShader")->SetInteger("screenTexture", 0);

		s_Data.Camera = cameraRenderer;
		s_Data.CameraPosition = cameraPosition;
		s_Data.Camera->Resize(s_Framebuffer->GetSpecification().Width, s_Framebuffer->GetSpecification().Height);

		BeginBatch();
	}

	void Renderer2D::EndScene()
	{
		for (auto& drawData : s_Data.VertexData)
		{
			drawData.second->material->shader->Use();
			s_Data.Camera->SetShaderMatrices(drawData.second->material->shader, s_Data.CameraPosition);

			if (s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > (MaxTextures - 1))
			{
				EndBatch();
				Flush();
				BeginBatch();
			}

			for (int i = 0; i < 4; i++)
			{
				s_Data.QuadBufferPtr->Position = drawData.second->vertices[i].Position;
				s_Data.QuadBufferPtr->Color = drawData.second->vertices[i].Color;
				s_Data.QuadBufferPtr->TexCoords = drawData.second->vertices[i].TexCoords;
				s_Data.QuadBufferPtr->TexID = drawData.second->vertices[i].TexID;
				s_Data.QuadBufferPtr->EntityID = drawData.second->vertices[i].EntityID;
				s_Data.QuadBufferPtr++;
			}

			s_Data.IndexCount += 6;
		}

		EndBatch();
		Flush();

		s_Data.VertexData.clear();
		s_Framebuffer->Unbind();

#ifndef WV_DEBUG
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		AssetManager::GetShader("ScreenShader")->Use();
		glBindVertexArray(s_Data.SVAO);
		s_Framebuffer->BindColorAttachmentTexture();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
#endif
	}

	void Renderer2D::BeginBatch()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		s_Data.QuadBufferPtr = s_Data.QuadBuffer;
	}

	void Renderer2D::EndBatch()
	{
		GLsizeiptr size = (uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBuffer;
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data.QuadBuffer);
	}

	void Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			glBindTextureUnit(i, s_Data.TextureSlots[i]);

		glBindVertexArray(s_Data.VAO);
		glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		s_Data.IndexCount = 0;
	}

	void Renderer2D::DrawQuad(Transform* transform, Ref<Material> material, Ref<Sprite> sprite, const Vector4& color, int entityID)
	{
		if (material == nullptr) return;

		float textureIndex = 0.0f;
		Vector2 coords[4] = {
			Vector2(0.0f, 0.0f),
			Vector2(0.0f, 0.0f),
			Vector2(0.0f, 0.0f),
			Vector2(0.0f, 0.0f)
		};

		if (sprite && sprite->GetTexture())
		{
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (s_Data.TextureSlots[i] == sprite->GetTexture()->GetID())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = sprite->GetTexture()->GetID();
				s_Data.TextureSlotIndex++;
			}

			coords[0] = sprite->GetTexCoords()[0];
			coords[1] = sprite->GetTexCoords()[1];
			coords[2] = sprite->GetTexCoords()[2];
			coords[3] = sprite->GetTexCoords()[3];
		}

		Matrix4x4 matrix = Matrix4x4::Translate(transform->GlobalPosition()) * Matrix4x4::Rotate(transform->GlobalRotation());

		Vector3 v1 = matrix * glm::vec4(-transform->GlobalScale().x, -transform->GlobalScale().y, 0.0f, 1.0f);
		Vector3 v2 = matrix * glm::vec4(transform->GlobalScale().x, -transform->GlobalScale().y, 0.0f, 1.0f );
		Vector3 v3 = matrix * glm::vec4(transform->GlobalScale().x, transform->GlobalScale().y, 0.0f, 1.0f );
		Vector3 v4 = matrix * glm::vec4(-transform->GlobalScale().x, transform->GlobalScale().y, 0.0f, 1.0f );

		Vertex vertices[4] = {
			Vertex(
				v1,
				color,
				coords[3],
				textureIndex,
				entityID
			),
			Vertex(
				v2,
				color,
				coords[2],
				textureIndex,
				entityID
			),
			Vertex(
				v3,
				color,
				coords[1],
				textureIndex,
				entityID
			),
			Vertex(
				v4,
				color,
				coords[0],
				textureIndex,
				entityID
			)
		};
		
		Ref<VertexArray> vertexArray = CreateRef<VertexArray>(vertices, material);
		s_Data.VertexData.insert(std::pair<float, Ref<VertexArray>>(transform->position.z, vertexArray));
	}

	void Renderer2D::DrawText(Vector3 pos, const Vector2& size, const std::string& text)
	{
		Ref<Material> fontMaterial = AssetManager::GetMaterial("StandardFontMaterial");
		std::string::const_iterator c;

		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			float textureIndex = 0.0f;
			for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			{
				if (s_Data.TextureSlots[i] == ch.TextureID)
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = ch.TextureID;
				s_Data.TextureSlotIndex++;
			}

			float xpos = pos.x + ch.Bearing.x * size.x;
			float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * size.y;

			float width = (ch.Size.x * size.x) * 0.5f;
			float height = (ch.Size.y * size.y) * 0.5f;

			Vertex vertices[4];

			vertices[0] = Vertex(
				{ xpos - width, ypos - height, pos.z },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f },
				textureIndex
			);
			vertices[1] = Vertex(
				{ xpos + width, ypos - height, pos.z },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f },
				textureIndex
			);
			vertices[2] = Vertex(
				{ xpos + width, ypos + height, pos.z },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f },
				textureIndex
			);
			vertices[3] = Vertex(
				{ xpos - width, ypos + height, pos.z },
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f },
				textureIndex
			);

			Ref<VertexArray> vertexArray = CreateRef<VertexArray>(vertices, fontMaterial);
			s_Data.VertexData.insert(std::pair<float, Ref<VertexArray>>(pos.z, vertexArray));

			pos.x += (ch.Advance >> 6) * size.x;
		}
	}
}