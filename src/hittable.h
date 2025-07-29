#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"


namespace render {

    struct HitRecord {
        HitRecord(double t, point3 p, vec3 n, Ray r) : t(t),p(p){
            if (glm::dot(r.direction(),n)>=0){
                normal = -n;
                front_face = false;
            }
            else{
                normal = n;
                front_face = true;
            }
        }
        double t;
        point3 p;
        vec3 normal;
        bool front_face;
    };

    // class hittable {
    //   public:
    //     virtual ~hittable() = default;

    //     virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
    // };

    struct Sphere {
        point3 center;
        double radius;
        vec3 normal(const vec3& point) const {
            return(point - center) / radius;
        }
    };


}
#endif