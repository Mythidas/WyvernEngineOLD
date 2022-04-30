#pragma once

#include <Wyvern.h>

namespace Wyvern::Editor
{
	class EditorGUIInternal
	{
	public:
		static void DrawComponent(const std::string label, Component* component, Entity* entity, bool isDefault = false);
	};
}