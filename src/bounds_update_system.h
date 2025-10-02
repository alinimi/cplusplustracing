#ifndef BOUNDS_UPDATE_SYSTEM_H
#define BOUNDS_UPDATE_SYSTEM_H

#include "geometry/hittable.h"
#include "geometry/bounds.h"
#include "ecs/system.h"
#include "ecs/view.h"

namespace geom {
    using BoundsView = View<geom::Sphere, geom::Bounds>;
    class BoundsUpdateSystem :public System {
    public:
        void update_bounds(BoundsView& view) const {
            for (const auto& [e, sphere, bounds] : view)
            {
                vec3 minp = sphere.center - vec3(sphere.radius, sphere.radius, sphere.radius);
                vec3 maxp = sphere.center + vec3(sphere.radius, sphere.radius, sphere.radius);
                if (near_zero(sphere.direction)) {
                    bounds = Bounds(minp, maxp);
                }
                else {
                    bounds = Bounds(
                        Bounds(minp, maxp),
                        Bounds(minp + sphere.direction, maxp + sphere.direction)
                    );
                }
            }
        }



    private:
        int m_channels = 3; // Number of color channels (R, G, B)
    };
}

#endif // BOUNDS_UPDATE_SYSTEM_H