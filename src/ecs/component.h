#ifndef COMPONENT_H
#define COMPONENT_H

#include <unordered_map>
#include "entity.h"


constexpr Entity INVALID = std::numeric_limits<Entity>::max();

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
    template <typename T>
    void registerComponent() {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) == m_componentTypes.end() && "Component already registered.");
        m_componentTypes[typeName] = m_nextComponentType;
        m_componentArrays[m_nextComponentType] = std::make_unique<ComponentArray<T>>();
        ++m_nextComponentType;
    }

    template <typename T>
    ComponentType getComponentType() {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered.");
        return m_componentTypes[typeName];
    }


    template <typename T>
    T& getComponent(Entity entity) {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered.");
        return getComponentArray<T>()->getData(entity);
    }



    template <typename T>
    void addComponent(Entity entity, T component) {
        const char* typeName = typeid(T).name();
        getComponentArray<T>()->insertData(entity, component);

    }

    template <typename T>
    void removeComponent(Entity entity) {
        const char* typeName = typeid(T).name();
        getComponentArray<T>()->removeData(entity);
    }

    void entityDestroyed(Entity entity) {
        for (auto& pair : m_componentArrays) {
            pair.second->EntityDestroyed(entity);
        }
    }

private:
    template <typename T>
    ComponentArray<T>* getComponentArray() {
        const char* typeName = typeid(T).name();
        assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Component not registered.");
        return static_cast<ComponentArray<T>*>(m_componentArrays[m_componentTypes[typeName]].get());
    }

    std::unordered_map<const char*, ComponentType> m_componentTypes{}; // Maps component type to its ID
    std::unordered_map<ComponentType, std::unique_ptr<IComponentArray>> m_componentArrays{}; // Maps component ID to its array
    ComponentType m_nextComponentType = 0; // Next available component type ID
};

#endif