#ifndef SYSTEM_H
#define SYSTEM_H

#include <memory>

using SystemType = std::uint8_t;
inline SystemType nextSystemID = 0;
const SystemType MAX_SYSTEMS = 32;


class System {
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

    void entityChanged() {

    }

private:
    std::array<std::unique_ptr<System>, MAX_SYSTEMS> m_systems{}; // Maps system type to its instance

};


#endif