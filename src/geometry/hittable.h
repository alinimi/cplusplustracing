#ifndef HITTABLE_H
#define HITTABLE_H


#include "../common.h"
namespace geom {
    struct Sphere {
        point3 center;
        double radius;
        vec3 direction{ 0.,0.,0. };
    };
}
#endif