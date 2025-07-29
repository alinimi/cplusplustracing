#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"


namespace render {
    struct Sphere {
        point3 center;
        double radius;
        constexpr vec3 normal(const vec3 point) const {
            return(point - center) / radius;
        }
    };
}
#endif