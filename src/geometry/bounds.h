#ifndef BOUNDS_H
#define BOUNDS_H

#include "../common.h"
#include "interval.h"

namespace geom {
    // TODO: Clarify why the separation between Bounds and AABB exists: will we use local coordinates
    // for something? move AABB functionality to AABB
    // using AABB = std::array<Interval, 3>;
    struct AABB {
        Interval operator [](int dim) const {
            switch (dim) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                assert(false && "Accessing invalid dimension of AABB");
                throw;
            }
        }

        int longest_axis() {
            if (x.size() > y.size())
                return x.size() > z.size() ? 0 : 2;
            else
                return y.size() > z.size() ? 1 : 2;
        }

        Interval x;
        Interval y;
        Interval z;
    };

    struct Bounds {
        constexpr Bounds() {}
        constexpr Bounds(AABB wb) : world_bounds(wb) {}
        constexpr Bounds(const vec3& minp, const vec3& maxp) :
            world_bounds({ Interval(minp[0],maxp[0]),Interval(minp[1],maxp[1]),Interval(minp[2],maxp[2]) })
        {
        }
        constexpr Bounds(const Bounds& b1, const Bounds& b2)
        {
            world_bounds = {
                Interval(b1.world_bounds.x, b2.world_bounds.x),
                Interval(b1.world_bounds.y, b2.world_bounds.y),
                Interval(b1.world_bounds.z, b2.world_bounds.z)
            };
        }
        constexpr Bounds padded()
        {
            return  {
                {
                    Interval(world_bounds.x, world_bounds.x).expand(1e-5),
                    Interval(world_bounds.y, world_bounds.y).expand(1e-5),
                    Interval(world_bounds.z, world_bounds.z).expand(1e-5)
                }
            };
        }
        AABB world_bounds;
    };

}


#endif