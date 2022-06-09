#pragma once

#include <Wyvern/Core/Scene/Component.h>
#include <Wyvern/Core/Graphics/Material.h>
#include <Wyvern/Core/Components/MeshFilter.h>
#include <Wyvern/Core/AssetManager.h>

namespace Wyvern
{
	struct MeshRenderer : public Component
	{
	public:
		std::vector<UUID> materials = std::vector<UUID>{ AssetManager::GetDefaultMaterial()->uuid };

		WV_SERIALIZE_COMPONENT(MeshRenderer)
		WV_SERIALIZE_COMPONENT_END
	};
}