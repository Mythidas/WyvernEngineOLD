#include "wvpch.h"
#include "SerializeHelper.h"

namespace Wyvern
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const PhysicsBody& body)
	{
		out << (int)body;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Renderer::CameraMode& cameraMode)
	{
		out << (int)cameraMode;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Sprite& sprite)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;

		out << sprite.GetTexture()->GetPath();
		out << sprite.GetTexCoords()[0];
		out << sprite.GetTexCoords()[1];
		out << sprite.GetTexCoords()[2];
		out << sprite.GetTexCoords()[3];

		out << YAML::EndSeq;
		return out;
	}
}