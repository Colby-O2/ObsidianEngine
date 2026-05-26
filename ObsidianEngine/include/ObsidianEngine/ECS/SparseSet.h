#ifndef __OBSIDIANENGINE_ECS_SPARSE_SET_HPP__
#define __OBSIDIANENGINE_ECS_SPARSE_SET_HPP__

#include "Entity.h"

#include <vector>
#include <cstdint>

namespace ObsidianEngine
{
	class SparseSet
	{
	public:
		void add(EntityID entity);
		void remove(EntityID entity);
		bool contains(EntityID entity) const;
		uint32_t getIndex(EntityID entity) const;
		const std::vector<EntityID>& dense() const;
		size_t size() const;
	private:
		std::vector<uint32_t> m_sparse;
		std::vector<EntityID> m_dense;
	};
}

#endif