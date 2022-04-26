#pragma once

#include <Merlin/Core/Scene/Component.h>

#include <map>
#include <memory>

namespace Merlin
{
	class ApplicationDomain
	{
	public:
		using CreateComponentFn = std::shared_ptr<Component>(*)();

	public:
		ApplicationDomain() = delete;

		static bool RegisterComponent(std::string name, CreateComponentFn func);
		static std::shared_ptr<Component> CreateComponent(std::string name);

	private:
		static inline std::map<std::string, CreateComponentFn> s_Components;
	};
}