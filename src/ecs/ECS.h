#ifndef ECS_H
#define ECS_H
#include <cassert>
#include <stdexcept>
#include <memory>
#include "entity.h"
#include "component.h"
#include "system.h"

// TODO: Create const getters for read-only systems

class ECS {
public:
    ECS() : m_entityManager(), m_componentManager(), m_systemManager() {};
    Entity createEntity() {
        return m_entityManager.createEntity();
    }
    void destroyEntity(Entity entity) {
        m_entityManager.destroyEntity(entity);
        m_componentManager.entityDestroyed(entity);
        m_systemManager.entityDestroyed(entity);
    }
    template <typename T>
    void registerComponent() {
        m_componentManager.registerComponent<T>();
    }
    template <typename T>
    void addComponent(Entity entity, T component) {
        m_componentManager.addComponent<T>(entity, component);

        auto signature = m_entityManager.getSignature(entity);
        signature.set(m_componentManager.getComponentType<T>(),
            true);
        m_entityManager.setSignature(entity, signature);
        m_systemManager.EntitySignatureChanged(entity, m_entityManager.getSignature(entity));
    }

    template <typename T>
    void removeComponent(Entity entity) {
        m_componentManager.removeComponent<T>(entity);
        auto signature = m_entityManager.getSignature(entity);
        signature.set(m_componentManager.getComponentType<T>(), false);
        m_entityManager.setSignature(entity, signature);
        m_systemManager.EntitySignatureChanged(entity, m_entityManager.getSignature(entity));
    }
    template <typename T>
    T& getComponent(Entity entity) {
        return m_componentManager.getComponent<T>(entity);
    }
    template <typename T>
    const T& getComponent(Entity entity) const {
        return m_componentManager.getComponent<T>(entity);
    }
    template <typename T>
    ComponentType getComponentType() {
        return m_componentManager.getComponentType<T>();
    }
    template <typename T>
    std::shared_ptr<T> registerSystem() {
        return m_systemManager.registerSystem<T>();
    }
    template <typename T>
    void setSystemSignature(Signature signature) {
        m_systemManager.setSignature<T>(signature);
    }

private:
    EntityManager m_entityManager; // Manages entities
    ComponentManager m_componentManager; // Manages components
    SystemManager m_systemManager; // Manages systems
};

#endif // ECS_H