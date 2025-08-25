#ifndef COMPONENT_H
#define COMPONENT_H

#include <unordered_map>
#include "entity.h"


class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0; // Remove component from entity

};

template <typename T>
class ComponentArray : public IComponentArray {
public:
    void insertData(Entity entity, T component) {

        assert(m_entityToIndex.find(entity) == m_entityToIndex.end() && "Component added to same entity more than once.");
        size_t newIndex = m_size;
        m_componentArray[newIndex] = component;
        m_entityToIndex[entity] = newIndex;
        m_indexToEntity[newIndex] = entity;
        ++m_size;
    }
    void removeData(Entity entity) {
        assert(m_entityToIndex.find(entity) != m_entityToIndex.end() && "Removing non-existent component.");
        size_t index = m_entityToIndex[entity];
        size_t lastIndex = m_size - 1;
        if (index != lastIndex) {
            // Move the last element to the index of the removed element
            Entity lastEntity = m_indexToEntity[lastIndex];
            m_componentArray[index] = m_componentArray[lastIndex];
            m_entityToIndex[lastEntity] = index;
            m_indexToEntity[index] = lastEntity;
        }
        m_componentArray[index] = T(); // Reset the component
        m_entityToIndex.erase(entity);
        m_indexToEntity.erase(index);
        --m_size;
    }

    T& getData(Entity entity) {
        assert(m_entityToIndex.find(entity) != m_entityToIndex.end() && "Component not found for entity.");
        return m_componentArray[m_entityToIndex[entity]];
    }

    void EntityDestroyed(Entity entity) override {

        if (m_entityToIndex.find(entity) != m_entityToIndex.end()) {
            removeData(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> m_componentArray; // Array of components
    std::unordered_map<Entity, size_t> m_entityToIndex; // Maps index to entity
    std::unordered_map<size_t, Entity> m_indexToEntity; // Maps entity to index
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