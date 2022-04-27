#pragma once

#include "EntityConstants.h"
#include "ComponentPool.h"
#include "WizardStack.h"

#include <Merlin/Events/Event.h>
#include <Merlin/Core/Components/Components.h>
#include <Merlin/Core/Debug.h>

#include <vector>
#include <typeinfo>
#include <unordered_map>

namespace Merlin
{
	class Entiy;

	enum class SceneState
	{
		Play,
		Edit
	};

	class Scene
	{
		template <class... ComponentTypes>
		friend struct EntityList;
		friend class SceneSerializer;
	public:
		static void OnAwake();
		static void OnDestroy();
		static void OnRuntimeUpdate();
		static void OnEditorUpdate(Renderer::CameraRenderer* camera, const Matrix4x4& position);
		static void OnFixedUpdate();
		static void OnEvent(Events::Event& e);

		static void SetSceneState(SceneState state);

		static void FlushScene();
	public:
		template <typename T>
		static T* CreateEntity(std::string name = "Entity");
		static void DestoryEntity(Entity* ent);

		static EntityIndex GetEntityIndex(EntityID id);
		static EntityVersion GetEntityVersion(EntityID id);
		static bool IsEntityValid(EntityID id);
		static bool IsEntityValid(Entity* ent);

	public:
		template <typename T>
		static T* AddComponent(Entity* ent);
		template <typename T>
		static T* GetComponent(Entity* ent);
		template <typename T>
		static std::vector<T*> GetComponentsOfBase(Entity* ent);
		template <typename T>
		static void RemoveComponent(Entity* ent);

		template <typename T>
		static int GetComponentID();
		template <typename T>
		static inline int FindComponentID(bool isBase = false);

		static size_t GetEntityCount() { return s_Entities.size(); }
		
	public:
		template <typename T>
		static void CreateWizard();
		template <typename T>
		static void DestroyWizard();

	private:
		static SceneState s_SceneState;

	private:
		static inline std::vector<Entity*> s_Entities;
		static inline std::vector<EntityIndex> s_FreeEntities;
		static inline std::vector<ComponentPool*> s_ComponentPools;

		static inline std::vector<Entity*> s_EntitiesToDelete;
		static inline std::unordered_map<Entity*, int> s_ComponentsToDelete;

		static inline int s_ComponentCounter;

		static EntityID CreateEntityID(EntityIndex index, EntityVersion version);

		static void PurgeEntity(Entity* ent);
		static void PurgeComponent(Entity* ent, int component);

	private:
		static WizardStack s_WizardStack;
	};

	template <typename T>
	inline T* Scene::CreateEntity(std::string name)
	{
		if (s_Entities.size() >= MaxEntities)
		{
			DEBUG_LOG_ERROR("Scene: MaxEntity count reached; ", MaxEntities);
			throw std::invalid_argument("Scene: MaxEntity count reached");
			return nullptr;
		}
		if (!std::is_base_of<Entity, T>())
		{
			DEBUG_LOG_ERROR("Scene: Entity or Inherited class required: ", typeid(T).name(), " was provided");
			throw std::invalid_argument("Scene: Type provided was not Entity");
			return nullptr;
		}

		if (!s_FreeEntities.empty())
		{
			EntityIndex index = s_FreeEntities.back();
			s_FreeEntities.pop_back();
			EntityID id = CreateEntityID(index, GetEntityVersion(s_Entities[index]->m_ID));
			s_Entities[index]->m_ID = id;
			s_Entities[index]->m_Components.reset();
			Transform* ts = AddComponent<Transform>(s_Entities[index]);
			Tag* tag = AddComponent<Tag>(s_Entities[index]);
			tag->name = name;
			s_Entities[index]->m_Transform = ts;
			s_Entities[index]->m_Tag = tag;
			s_Entities[index]->OnAttach();
			return (T*)s_Entities[index];
		}

		T* entity = new T();
		entity->m_ID = CreateEntityID((EntityIndex)s_Entities.size(), 0);
		entity->m_Components = ComponentMask();

		s_Entities.push_back(entity);
		s_Entities.back()->m_Components.reset();

		Transform* ts = AddComponent<Transform>(s_Entities.back());
		Tag* tag = AddComponent<Tag>(s_Entities.back());
		tag->name = name;
		s_Entities.back()->m_Transform = ts;
		s_Entities.back()->m_Tag = tag;
		s_Entities.back()->OnAttach();

		return (T*)s_Entities.back();
	}

	template <typename T>
	inline T* Scene::AddComponent(Entity* ent)
	{
		if (!IsEntityValid(ent)) return nullptr;

		if (!std::is_base_of<Component, T>())
		{
			DEBUG_LOG_ERROR("Scene: Component or Inherited class required: ", typeid(T).name(), " was provided");
			throw std::invalid_argument("Scene: Type provided was not Component");
			return nullptr;
		}

		int componentID = GetComponentID<T>();

		if (ent->m_Components.test(componentID))
		{
			DEBUG_CORE_ERROR("Scene: Component ", typeid(T).name(), " already exists on this entity");
			return GetComponent<T>(ent);
		}

		if (s_ComponentPools.size() <= componentID)
		{
			s_ComponentPools.resize(componentID + 1, nullptr);
		}

		if (s_ComponentPools[componentID] == nullptr)
		{
			s_ComponentPools[componentID] = new ComponentPool(sizeof(T), typeid(T).name(), typeid(T::base).name(), componentID);
		}

		T* component = new (s_ComponentPools[componentID]->Get(GetEntityIndex(ent->m_ID))) T();
		component->m_Entity = ent;
		component->m_Transform = ent->m_Transform;
		component->m_Tag = ent->m_Tag;

		ent->m_Components.set(componentID);
		ent->m_ComponentPtrs.push_back(component);

		return component;
	}

	template <typename T>
	inline T* Scene::GetComponent(Entity* ent)
	{
		if (!IsEntityValid(ent)) return nullptr;

		int componentID = FindComponentID<T>();

		if (!ent->m_Components.test(componentID))
			return nullptr;

		T* component = static_cast<T*>(s_ComponentPools[componentID]->Get(GetEntityIndex(ent->m_ID)));
		return component;
	}

	template<typename T>
	inline std::vector<T*> Scene::GetComponentsOfBase(Entity* ent)
	{
		std::vector<T*> components;

		for (ComponentPool* pool : s_ComponentPools)
		{
			if (pool->ComponentBaseType == typeid(T).name())
				if (ent->m_Components.test(pool->ComponentID))
					components.push_back(static_cast<T*>(pool->Get(GetEntityIndex(ent->m_ID))));
		}

		return components;
	}

	template <typename T>
	inline void Scene::RemoveComponent(Entity* ent)
	{
		s_ComponentsToDelete[ent] = FindComponentID<T>();
	}

	template<typename T>
	inline int Scene::GetComponentID()
	{
		for (ComponentPool* pool : s_ComponentPools)
			if (pool->ComponentType == typeid(T).name())
				return pool->ComponentID;

		int index = s_ComponentCounter++;
		return index;
	}

	template <typename T>
	static int Scene::FindComponentID(bool isBase)
	{
		for (ComponentPool* pool : s_ComponentPools)
			if (isBase ? pool->ComponentBaseType == typeid(T::base).name() : pool->ComponentType == typeid(T).name())
				return pool->ComponentID;

		return -1;
	}

	template<typename T>
	inline void Scene::CreateWizard()
	{
		s_WizardStack.PushWizard(new T());
	}

	template<typename T>
	inline void Scene::DestroyWizard()
	{
		// Add removal of Wizard
	}
}