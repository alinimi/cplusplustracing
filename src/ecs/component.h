#ifndef COMPONENT_H
#define COMPONENT_H

#include "entity.h"

inline ComponentType nextComponentID = 0;

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0; // Remove component from entity

};

template <typename T>
class ComponentArray : public IComponentArray {
public:

    ComponentArray() {
        m_sparse.fill(INVALID);
    }

    bool hasComponent(Entity entity) {
        return m_sparse[entity] != INVALID;
    }

    void insertData(Entity entity, T component) {
        assert(!hasComponent(entity) && "Component added to same entity more than once.");
        m_componentArray[m_size] = component;
        m_sparse[entity] = m_size;
        m_dense[m_size] = entity;
        ++m_size;
    }

    void removeData(Entity entity) {
        assert(hasComponent(entity) && "Removing non-existent component.");
        size_t index = m_sparse[entity];
        size_t lastIndex = m_size - 1;
        if (index != lastIndex) {
            // Move the last element to the index of the removed element
            m_dense[index] = m_dense[lastIndex];
            m_sparse[m_dense[index]] = index;
            m_componentArray[index] = m_componentArray[lastIndex];
        }
        m_componentArray[index] = T(); // Reset the component
        m_sparse[index] = INVALID;
        --m_size;
    }

    T& getData(Entity entity) {
        assert(hasComponent(entity) && "Component not found for entity.");
        return m_componentArray[m_sparse[entity]];
    }

    void EntityDestroyed(Entity entity) override {
        if (hasComponent(entity)) {
            removeData(entity);
        }
    }

    size_t size() {
        return m_size;
    }

private:
    std::array<T, MAX_ENTITIES> m_componentArray; // Array of components
    std::array<Entity, MAX_ENTITIES> m_dense{};
    std::array<size_t, MAX_ENTITIES> m_sparse{}; // maps entity -> index in dense
    size_t m_size = 0; // Current size of the component array




};

class ComponentManager {
public:
    template<typename T>
    bool isRegistered() const {
        auto componentID = getComponentType<T>();
        return m_componentArrays[componentID] != nullptr;
    }

    template <typename T>
    void registerComponent() {
        assert(!isRegistered<T>() && "Component already registered.");
        assert(getComponentType<T>() < m_componentArrays.size() && "Max number of components reached.");
        m_componentArrays[getComponentType<T>()] = std::make_unique<ComponentArray<T>>();
    }

    template <typename T>
    ComponentType getComponentType() const {
        static ComponentType typeID = nextComponentID++;
        return typeID;
    }

    template <typename T>
    T& getComponent(Entity entity) {
        assert(isRegistered<T>() && "Component not registered.");
        return getComponentArray<T>().getData(entity);
    }

    template <typename T>
    void addComponent(Entity entity, T component) {
        getComponentArray<T>().insertData(entity, component);
    }

    template <typename T>
    void removeComponent(Entity entity) {
        getComponentArray<T>().removeData(entity);
    }

    void entityDestroyed(Entity entity) {
        for (auto& compArray : m_componentArrays) {
            compArray->EntityDestroyed(entity);
        }
    }

private:
    template <typename T>
    ComponentArray<T>& getComponentArray() {
        assert(isRegistered<T>() && "Component not registered.");
        return *(static_cast<ComponentArray<T>*>(m_componentArrays[getComponentType<T>()].get()));
    }
    std::array<std::unique_ptr<IComponentArray>, MAX_COMPONENTS> m_componentArrays{}; // Maps component ID to its array
};

#endif