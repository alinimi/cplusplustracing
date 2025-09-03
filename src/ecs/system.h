#ifndef SYSTEM_H
#define SYSTEM_H
#include <set>
#include <memory>
#include "entity.h"

using SystemType = std::uint8_t;
inline SystemType nextSystemID = 0;
const SystemType MAX_SYSTEMS = 32;


class System {
public:
    std::set<Entity> entities; // Set of entities that this system operates on
};


class SystemManager {
public:
    template <typename T>
    SystemType getSystemType() const {
        static SystemType typeID = nextSystemID++;
        return typeID;
    }

    template<typename T>
    bool isRegistered() const {
        auto systemID = getSystemType<T>();
        return m_systems[systemID] != nullptr;
    }

    template <typename T>
    T& registerSystem() {
        assert(!isRegistered<T>() && "System already registered.");
        assert(getSystemType<T>() < m_systems.size() && "Max number of systems reached.");
        m_systems[getSystemType<T>()] = std::make_unique<T>();
        return *(static_cast<T*>(m_systems[getSystemType<T>()].get()));
    }

    template <typename T>
    void setSignature(Signature signature) {
        assert(isRegistered<T>() && "System not registered.");
        m_signatures[getSystemType<T>()] = signature;
    }
    void entityDestroyed(Entity entity) {
        for (auto& system : m_systems) {
            system->entities.erase(entity);
        }
    }
    void EntitySignatureChanged(Entity entity, Signature signature) {
        for (int i = 0; i < nextSystemID; ++i) {
            if ((signature & m_signatures[i]) == m_signatures[i]) {
                m_systems[i]->entities.insert(entity);
            }
            else {
                m_systems[i]->entities.erase(entity);
            }
        }
    }

private:
    std::array<std::unique_ptr<System>, MAX_SYSTEMS> m_systems{}; // Maps system type to its instance
    std::array<Signature, MAX_SYSTEMS> m_signatures{};



};


#endif