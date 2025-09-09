#ifndef ENTITY_H
#define ENTITY_H
#include <cstdint>
#include <queue>
#include <array>
#include <bitset>

using Entity = uint32_t;
using ComponentType = std::uint8_t;

const ComponentType MAX_COMPONENTS = 32;


// Used to define the size of arrays later on
constexpr Entity MAX_ENTITIES = 5000;
constexpr Entity INVALID = std::numeric_limits<Entity>::max();


class EntityManager {
public:
    EntityManager() {
        for (Entity i = 0; i < MAX_ENTITIES; ++i) {
            m_availableEntities.push(i);
        }
    }

    Entity createEntity() {
        if (m_availableEntities.empty()) {
            throw std::runtime_error("No available entities");
        }
        Entity entity = m_availableEntities.front();
        m_availableEntities.pop();
        return entity;
    }

    void destroyEntity(Entity entity) {
        if (entity >= MAX_ENTITIES) {
            throw std::out_of_range("Entity out of range");
        }
        m_availableEntities.push(entity);
    }

private:
    std::queue<Entity> m_availableEntities; // Queue of available entities
};


#endif