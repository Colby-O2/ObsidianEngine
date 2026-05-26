#ifndef __OBSIDIANENGINE_ECS_COMPONENT_POOL_HPP__
#define __OBSIDIANENGINE_ECS_COMPONENT_POOL_HPP__

#include "Entity.h"
#include "SparseSet.h"

#include <vector>
#include <utility>

namespace ObsidianEngine
{
	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;
		virtual void remove(EntityID entity) = 0;
	};

	template<typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		template<typename... Args>
		T& emplace(EntityID entity, Args&&... args)
		{
			if (m_set.contains(entity))
			{
				uint32_t index = m_set.getIndex(entity);

				m_data[index] = T(std::forward<Args>(args)...);
				return m_data[index];
			}

			m_set.add(entity);

			m_data.emplace_back(std::forward<Args>(args)...);
			return m_data.back();
		}

		void remove(EntityID entity) override
		{
			if (!m_set.contains(entity)) return;

			uint32_t index = m_set.getIndex(entity);

			m_data[index] = std::move(m_data.back());
			m_data.pop_back();

			m_set.remove(entity);
		}

		T& get(EntityID entity)
		{
			return m_data[m_set.getIndex(entity)];
		}

		bool contains(EntityID entity) const 
		{ 
			return m_set.contains(entity); 
		}

		const std::vector<EntityID>& entities() const 
		{ 
			return m_set.dense(); 
		}

	private:
		SparseSet m_set;
		std::vector<T> m_data;
	};
}

#endif