#include "bounds_update_system.h"
#include "geometry/geometry.h"

namespace geom {

    void BoundsUpdateSystem::update_bounds(BoundsView& view) const {
        for (const auto& [sphere, bounds] : view)
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
}