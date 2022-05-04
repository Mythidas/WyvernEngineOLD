#pragma once

#include "Framebuffer.h"
#include "CameraRenderer.h"

#include <Wyvern/Core/Graphics/Sprite.h>
#include <Wyvern/Core/Graphics/Material.h>
#include <Wyvern/Core/Components/Transform.h>
#include <Wyvern/Core/Math/Vector.h>

namespace Wyvern::Renderer
{
	class Renderer2D
	{
	public:
		static void OnAwake();
		static void OnDestroy();

		static void BeginScene(CameraRenderer* cameraRenderer, const Matrix4x4& cameraPosition);
		static void EndScene();

		static void DrawQuad(Transform* transform, Ref<Material> material, Ref<Sprite> sprite, const Vector4& color);
		static void DrawText(Vector3 pos, const Vector2& size, const std::string& text);
	private:
		static void BeginBatch();
		static void EndBatch();
		static void Flush();
	};
}