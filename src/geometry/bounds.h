#ifndef BOUNDS_H
#define BOUNDS_H

#include "../common.h"
#include "interval.h"

namespace geom {

    using AABB = std::array<Interval, 3>;

    struct Bounds {
        constexpr Bounds() {}
        constexpr Bounds(const vec3& minp, const vec3& maxp) :
            world_bounds({ Interval(minp[0],maxp[0]),Interval(minp[1],maxp[1]),Interval(minp[2],maxp[2]) })
        {
        }
        constexpr Bounds(const Bounds& b1, const Bounds& b2)
        {
            world_bounds = {
                Interval(b1.world_bounds[0], b2.world_bounds[0]),
                Interval(b1.world_bounds[1], b2.world_bounds[1]),
                Interval(b1.world_bounds[2], b2.world_bounds[2])
            };
        }
        AABB world_bounds;
    };

}


#endif