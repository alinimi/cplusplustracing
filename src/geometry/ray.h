#ifndef RAY_H
#define RAY_H


#include "../common.h"


namespace render {



    class Ray {
    public:
        constexpr Ray(const point3 origin, const vec3 direction) : origin(origin), direction(direction), attenuation(0.,0.,0.), index(-1), depth(-1) {}

        constexpr point3 at(double t) const {
            return origin + t * direction;
        }
        const point3 origin;
        const vec3 direction;
        color attenuation;
        int index;
        int depth;
    };


}

#endif // RAY_H