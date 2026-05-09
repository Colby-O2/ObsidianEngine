#ifndef SPARSE_SET_H_
#define SPARSE_SET_H_

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