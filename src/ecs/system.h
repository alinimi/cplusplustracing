#ifndef SYSTEM_H
#define SYSTEM_H
#include <unordered_map>
#include <set>
#include <memory>
#include "entity.h"


class System {
public:
    std::set<Entity> entities; // Set of entities that this system operates on
};


class SystemManager {
public:
    template <typename T>
    std::shared_ptr<T> registerSystem() {
        const char* typeName = typeid(T).name();
        assert(m_systems.find(typeName) == m_systems.end() && "System already registered.");
        m_systems[typeName] = std::make_shared<T>();
        return std::static_pointer_cast<T>(m_systems[typeName]);

    }
    template <typename T>
    void setSignature(Signature signature) {
        const char* typeName = typeid(T).name();
        assert(m_systems.find(typeName) != m_systems.end() && "System not registered.");
        m_signatures[typeName] = signature;
    }
    void entityDestroyed(Entity entity) {
        for (auto& pair : m_systems) {
            pair.second->entities.erase(entity);
        }
    }
    void EntitySignatureChanged(Entity entity, Signature signature) {
        for (auto& pair : m_systems) {
            const char* typeName = pair.first;
            if ((signature & m_signatures[typeName]) == m_signatures[typeName]) {
                pair.second->entities.insert(entity);
            }
            else {
                pair.second->entities.erase(entity);
            }
        }
    }
private:
    std::unordered_map<const char*, std::shared_ptr<System>> m_systems{}; // Maps system type to its instance
    std::unordered_map<const char*, Signature> m_signatures{};



};


#endif