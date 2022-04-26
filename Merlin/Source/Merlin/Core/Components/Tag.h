#pragma once

#include <Merlin/Core/Scene/Component.h>
#include <Merlin/Core/ApplicationDomain.h>

#include <string>

namespace Merlin
{
	struct Tag : public Component
	{
		Tag() {}
		~Tag() {}

		std::string name;

		virtual void Serialize(YAML::Emitter& out) override;
		virtual void Deserialize(Entity* ent, YAML::Node& data) override;

		static std::shared_ptr<Component> RegisterComponent() { return std::make_shared<Tag>(); }
		static inline bool IsRegistered = ApplicationDomain::RegisterComponent("Tag", RegisterComponent);
	};
}