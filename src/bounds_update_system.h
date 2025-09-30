#ifndef BOUNDS_UPDATE_SYSTEM_H
#define BOUNDS_UPDATE_SYSTEM_H

#include "geometry/hittable.h"
#include "geometry/bounds.h"
#include "ecs/system.h"

namespace geom {
    using BoundsView = View<geom::Sphere, geom::Bounds>;
    class BoundsUpdateSystem :public System {
    public:
        void update_bounds(BoundsView& view) const;

    private:
        int m_channels = 3; // Number of color channels (R, G, B)
    };
}

#endif // BOUNDS_UPDATE_SYSTEM_H