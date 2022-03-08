#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

namespace Merlin::Renderer
{
	class Shader
	{
	public:
		unsigned int ID;

		Shader();
		Shader(const char* vertSource, const char* fragSource, const char* geoSource = nullptr);

		Shader& Use();
		static void Use(unsigned int id);

		void SetFloat(const char* name, float value);
		void SetInteger(const char* name, int value);
		void SetVector2f(const char* name, const glm::vec2& value);
		void SetVector3f(const char* name, const glm::vec3& value);
		void SetVector4f(const char* name, const glm::vec4& value);
		void SetMatrix4(const char* name, const glm::mat4& matrix);
		void SetIntArray(const char* name, const int count, const GLint* intArray);

		static void SetFloat(const char* name, unsigned int id, float value);
		static void SetInteger(const char* name, unsigned int id, int value);
		static void SetVector2f(const char* name, unsigned int id, const glm::vec2& value);
		static void SetVector3f(const char* name, unsigned int id, const glm::vec3& value);
		static void SetVector4f(const char* name, unsigned int id, const glm::vec4& value);
		static void SetMatrix4(const char* name, unsigned int id, const glm::mat4& matrix);
		static void SetIntArray(const char* name, unsigned int id, const int count, const GLint* intArray);
	private:
		void CheckCompileErrors(unsigned int object, std::string type);
	};
}