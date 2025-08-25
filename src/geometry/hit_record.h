#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "../common.h"
#include "../material/material.h"

namespace render {


    struct HitRecord {
        HitRecord(double t, point3 p, vec3 n, Ray r) : t(t), p(p) {
            if (glm::dot(r.direction, n) >= 0) {
                normal = -n;
                front_face = false;
            }
            else {
                normal = n;
                front_face = true;
            }
        }
        double t;
        point3 p;
        vec3 normal;
        bool front_face;
        Entity entity;
    };

}
#endif //HIT_RECORD_H
