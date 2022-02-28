#include "mlpch.h"
#include "Shader.h"

namespace Merlin
{
	Shader::Shader()
	{

	}

	Shader::Shader(const char* vertSource, const char* fragSource, const char* geoSource)
	{
		unsigned int sVert, sFrag, sGeo;

		sVert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVert, 1, &vertSource, NULL);
		glCompileShader(sVert);
		CheckCompileErrors(sVert, "VERTEX");

		sFrag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFrag, 1, &fragSource, NULL);
		glCompileShader(sFrag);
		CheckCompileErrors(sFrag, "FRAGMENT");

		if (geoSource != nullptr)
		{
			sGeo = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(sGeo, 1, &geoSource, NULL);
			glCompileShader(sGeo);
			CheckCompileErrors(sGeo, "GEOMETRY");
		}

		ID = glCreateProgram();

		glAttachShader(ID, sVert);
		glAttachShader(ID, sFrag);
		if (geoSource != nullptr)
			glAttachShader(ID, sGeo);

		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");

		glDeleteShader(sVert);
		glDeleteShader(sFrag);
		if (geoSource != nullptr)
			glDeleteShader(sGeo);

	}

	Shader& Shader::Use()
	{
		glUseProgram(this->ID);
		return *this;
	}

	void Shader::SetFloat(const char* name, float value)
	{
		glUniform1f(glGetUniformLocation(ID, name), value);
	}

	void Shader::SetInteger(const char* name, int value)
	{
		glUniform1i(glGetUniformLocation(ID, name), value);
	}

	void Shader::SetVector2f(const char* name, const glm::vec2& value)
	{
		glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
	}

	void Shader::SetVector3f(const char* name, const glm::vec3& value)
	{
		glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
	}

	void Shader::SetVector4f(const char* name, const glm::vec4& value)
	{
		glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
	}

	void Shader::SetMatrix4(const char* name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(matrix));
	}



	void Shader::CheckCompileErrors(unsigned int object, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(object, 1024, NULL, infoLog);
				ML_LOG_ERROR("Shader: Compile-time error: Type: ", type, "\n", infoLog, "\n");
			}
		}
		else
		{
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(object, 1024, NULL, infoLog);
				ML_LOG_ERROR("Shader: Link-time error: Type: ", type, "\n", infoLog, "\n");
			}
		}
	}
}