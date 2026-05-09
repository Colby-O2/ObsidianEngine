#include "SparseSet.h"


namespace ObsidianEngine
{
	void SparseSet::add(EntityID entity)
	{
		if (contains(entity)) return;

		if (entity >= m_sparse.size())
		{
			m_sparse.resize(entity + 1, NULL_ENTITY);
		}

		m_sparse[entity] = static_cast<uint32_t>(m_dense.size());
		m_dense.push_back(entity);
	}

	void SparseSet::remove(EntityID entity)
	{
		if (!contains(entity)) return;

		uint32_t index = m_sparse[entity];
		EntityID lastEnitity = m_dense.back();

		m_dense[index] = lastEnitity;
		m_sparse[lastEnitity] = index;

		m_dense.pop_back();
		m_sparse[entity] = NULL_ENTITY;
	}

	bool SparseSet::contains(EntityID entity) const
	{
		return entity < m_sparse.size() && m_sparse[entity] != NULL_ENTITY;
	}

	uint32_t SparseSet::getIndex(EntityID entity) const
	{
		return m_sparse[entity];
	}

	const std::vector<EntityID>& SparseSet::dense() const
	{
		return m_dense;
	}

	size_t SparseSet::size() const
	{
		return m_dense.size();
	}
}