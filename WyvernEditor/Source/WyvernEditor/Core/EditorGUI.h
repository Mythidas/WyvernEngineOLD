#pragma once

#include <Wyvern/Core/Math/Vector.h>
#include <Wyvern/Core/Scene/Entity.h>

namespace Wyvern::Editor
{
	class EditorGUI
	{
	public:
		static void Vector2Control(const std::string& label, Vector2& values, float resetValue = 0.0f, float columnWidth = 60.0f);
		static void Vector3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 60.0f);
		static void Vector4Control(const std::string& label, Vector4& values, float resetValue = 0.0f, float columnWidth = 60.0f);

		static void Color4Control(const std::string& label, Vector4& values, float columnWidth = 60.0f);

		static void FloatControl(const std::string& label, float& value, float columnWidth = 60.0f);
		static void IntControl(const std::string& label, int& value, float columnWidth = 60.0f);
		static void BoolControl(const std::string& label, bool& value, float columnWidth = 60.0f);
		
		static void ComboControl(const std::string& label, const char* values[], int& valueIndex, int arraySize, float columnWidth = 60.0f);

		static void EntityControl(const std::string& label, Entity& entity, float columnWidth = 60.0f);
		static void FileSystemControl(const std::string& label, Tools::FileSystem& file, float columnWidth = 60.0f);
	};
}