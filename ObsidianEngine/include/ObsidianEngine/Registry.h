#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "Entity.h"
#include "ComponentPool.h"
#include "View.h"

#include <unordered_map>
#include <typeindex>

namespace ObsidianEngine
{
	class Registry
	{
	public:
		EntityID create() 
		{
			return m_entityCounter++;
		}

		void destroy(EntityID entity)
		{
			for (auto const& [type, pool] : m_pools)
			{
				pool->remove(entity);
			}
		}

		template<typename T, typename... Args>
		T& addComponent(EntityID entity, Args&&... args)
		{
			return getPool<T>()->emplace(
				entity,
				std::forward<Args>(args)...
			);
		}

		template<typename T>
		T& addExistingComponent(EntityID entity, const T& component)
		{
			return getPool<T>()->emplace(entity, component);
		}

		template<typename T>
		T& addExistingComponent(EntityID entity, T&& component)
		{
			return getPool<T>()->emplace(entity, std::move(component));
		}

		template<typename T>
		T& getComponent(EntityID entity) 
		{
			return getPool<T>()->get(entity);
		}

		template<typename T>
		bool hasComponent(EntityID entity) 
		{
			return getPool<T>()->contains(entity);
		}

		template<typename... Components>
		auto view();

		template<typename T>
		ComponentPool<T>* getPool() {
			auto type = std::type_index(typeid(T));
			if (m_pools.find(type) == m_pools.end()) 
			{
				m_pools[type] = std::make_unique<ComponentPool<T>>();
			}
			return static_cast<ComponentPool<T>*>(m_pools[type].get());
		}

	private:
		EntityID m_entityCounter = 0;
		std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_pools;
	};
}

#endif