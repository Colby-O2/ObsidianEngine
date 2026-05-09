#ifndef VIEW_H_
#define VIEW_H_

#include "Registry.h"

#include <tuple>

namespace ObsidianEngine {
    template<typename... Components>
    class View 
    {
    public:
        View(Registry& registry) : m_registry(registry), m_pools(std::make_tuple(registry.getPool<Components>()...)) {}

        struct Iterator 
        {
            EntityID operator*() const 
            { 
                return (*primaryPoolEntities)[index]; 
            }

            Iterator& operator++() 
            {
                index++;
                moveNext();
                return *this;
            }

            bool operator!=(const Iterator& other) const 
            { 
                return index != other.index; 
            }

            void moveNext() 
            {
                while (index < primaryPoolEntities->size()) {
                    EntityID entity = (*primaryPoolEntities)[index];
                    if ((std::get<ComponentPool<Components>*>(pools)->contains(entity) && ...)) {
                        break;
                    }
                    index++;
                }
            }

            size_t index;
            const std::vector<EntityID>* primaryPoolEntities;
            std::tuple<ComponentPool<Components>*...> pools;
        };

        Iterator begin() 
        {
            auto* primary = std::get<0>(m_pools);
            Iterator it{ 0, &primary->entities(), m_pools };
            it.moveNext();
            return it;
        }

        Iterator end() 
        {
            auto* primary = std::get<0>(m_pools);
            return { primary->entities().size(), &primary->entities(), m_pools };
        }

    private:
        Registry& m_registry;
        std::tuple<ComponentPool<Components>*...> m_pools;
    };

    template<typename... Components>
    auto Registry::view()
    {
        return View<Components...>(*this);
    }
}
#endif