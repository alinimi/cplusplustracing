#ifndef SYSTEM_H
#define SYSTEM_H
// #include <set>
#include <memory>
#include <tuple>
#include "entity.h"

using SystemType = std::uint8_t;
inline SystemType nextSystemID = 0;
const SystemType MAX_SYSTEMS = 32;

template<typename... Cs>
class View {
public:
    View()
    {
    }


    View(ComponentArray<Cs>&... storages)
        : storages{&storages...} 
    {
    }

    struct iterator {
        size_t index;
        View* view;

        bool operator!=(const iterator& other) const {
            return index != other.index;
        }

        void operator++() {
            while (++index < std::get<0>(view->storages)->dense().size()) {
                Entity e = std::get<0>(view->storages)->dense()[index];
                if (view->hasAll(e)) break;
            }
        }

        auto operator*() {
            Entity e = std::get<0>(view->storages)->dense()[index];
            return view->get(e);
        }
    };

    iterator begin() {
        size_t i = 0;
        while (i < std::get<0>(storages)->dense().size() && !hasAll(std::get<0>(storages)->dense()[i])) {
            ++i;
        }
        return {i, this};
    }

    iterator end() {
        return {std::get<0>(storages)->dense().size(), this};
    }

private:
    std::tuple<ComponentArray<Cs>*...> storages;

    bool hasAll(Entity e) {
        return (std::get<ComponentArray<Cs>*>(storages)->hasComponent(e) && ...);
    }

    auto get(Entity e) {
        return std::tuple<Cs&...>(std::get<ComponentArray<Cs>*>(storages)->getData(e)...);
    }
};

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

private:
    std::array<std::unique_ptr<System>, MAX_SYSTEMS> m_systems{}; // Maps system type to its instance

};


#endif