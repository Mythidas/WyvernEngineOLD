#pragma once

#include <string>

namespace Wyvern
{
	class WyvernObject
	{
	public:
		UUID uuid = UUID();

	public:
		virtual void _DrawProperties() {}

		template <typename T>
		bool _IsObjectType()
		{
			return std::string(typeid(T).name()) == std::string(typeid(*this).name());
		}

	public:
		template <typename T>
		static T* Cast(WyvernObject* object)
		{
			if (object && object->_IsObjectType<T>())
				return static_cast<T*>(object);
			else
				return nullptr;
		}
	};
}