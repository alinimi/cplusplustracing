#ifndef SYSTEM_H
#define SYSTEM_H
// #include <set>
#include <memory>
#include <tuple>
#include "entity.h"
#include "component.h"

using SystemType = std::uint8_t;
inline SystemType nextSystemID = 0;
const SystemType MAX_SYSTEMS = 32;

template<typename... Cs>
class View {
public:
    View() : dirty(true)
    {
    }

    View(ComponentArray<Cs>&... storages)
        : storages{ &storages... }, dirty(true)
    {
    }

    void entityChanged() {
        dirty = true;
    }

    struct iterator {
        size_t index;
        View* view;

        bool operator!=(const iterator& other) const {
            return index != other.index;
        }

        void operator++() {
            ++index;
        }

        auto operator*() {
            Entity e = std::get<0>(view->storages)->dense()[index];
            return view->get(e);
        }
    };

    struct const_iterator {
        size_t index;
        const View* const view;

        bool operator!=(const const_iterator& other) const {
            return index != other.index;
        }

        void operator++() {
            ++index;
        }

        auto operator*() const {
            Entity e = std::get<0>(view->storages)->dense()[index];
            return view->get(e);
        }
    };


    iterator begin() {
        ensureBuilt();
        return { 0, this };
    }
    iterator end() {
        ensureBuilt();
        return { entities.size(),this };
    }

    const_iterator begin() const {
        ensureBuilt();
        return { 0, this };
    }
    const_iterator end() const {
        ensureBuilt();
        return { entities.size(),this };
    }


private:
    std::tuple<ComponentArray<Cs>*...> storages;
    mutable std::vector<Entity> entities;
    mutable bool dirty;


    void build() const {
        entities.clear();
        for (const auto& entity : std::get<0>(storages)->dense()) {
            if (hasAll(entity)) {
                entities.push_back(entity);
            }
        }
    }

    void ensureBuilt() const {
        if (!dirty) return;
        build();
        dirty = false;
    }

    bool hasAll(Entity e) const {
        return (std::get<ComponentArray<Cs>*>(storages)->hasComponent(e) && ...);
    }

    auto get(Entity e) {
        return std::tuple<Cs&...>(std::get<ComponentArray<Cs>*>(storages)->getData(e)...);
    }

    auto get(Entity e) const {
        return std::tuple<Entity, Cs&...>(e, std::get<ComponentArray<Cs>*>(storages)->getData(e)...);
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

    void entityChanged() {

    }

private:
    std::array<std::unique_ptr<System>, MAX_SYSTEMS> m_systems{}; // Maps system type to its instance

};


#endif