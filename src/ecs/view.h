#ifndef VIEW_H
#define VIEW_H

#include "component.h"



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

    auto get(Entity e) const {
        return std::tuple<Entity, Cs&...>(e, std::get<ComponentArray<Cs>*>(storages)->getData(e)...);
    }

};


#endif