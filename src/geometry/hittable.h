#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"


namespace render {
    struct Sphere {
        point3 center;
        double radius;
        vec3 direction{ 0.,0.,0. };
    };
}
#endif