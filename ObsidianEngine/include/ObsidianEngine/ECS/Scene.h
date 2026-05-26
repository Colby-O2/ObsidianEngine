#ifndef __OBSIDIANENGINE_ECS_SCENE_HPP__
#define __OBSIDIANENGINE_ECS_SCENE_HPP__

#include "Entity.h"
#include "View.h"
#include "Registry.h"
#include "System.h" 
#include <vector>
#include <memory>
#include <string>

namespace ObsidianEngine
{
    class Scene 
    {
    public:
        Scene() = default;
        ~Scene() = default;

        EntityID createEntity() 
        {
            return m_registry.create();
        }

        void destroyEntity(EntityID entity) 
        {
            m_registry.destroy(entity);
        }

        template<typename T, typename... Args>
        void addSystem(Args&&... args) 
        {
            m_systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        template<typename T>
        T* getSystem()
        {
            for (auto& system : m_systems)
            {
                T* castedSystem = dynamic_cast<T*>(system.get());
                if (castedSystem != nullptr)
                {
                    return castedSystem;
                }
            }
            return nullptr;
        }

        void onUpdate(float dt) 
        {
            for (auto& system : m_systems) 
            {
                system->update(m_registry, dt);
            }
        }

        Registry& getRegistry() 
        { 
            return m_registry; 
        }

    private:
        Registry m_registry;
        std::vector<std::unique_ptr<ISystem>> m_systems;
    };
}
#endif