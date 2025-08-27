#ifndef RAY_H
#define RAY_H


#include "../common.h"


namespace render {



    class Ray {
    public:
        constexpr Ray(
            const point3 origin,
            const vec3 direction,
            const color attenuation,
            const int index,
            const int depth
        ) :
            origin(origin), direction(direction), attenuation(attenuation), index(index), depth(depth)
        {
        }

        constexpr Ray scattered(
            const point3 new_origin,
            const vec3 new_direction,
            const color new_attenuation
        ) const
        {
            return Ray(new_origin, new_direction, new_attenuation, index, depth - 1);
        }

        constexpr point3 at(double t) const {
            return origin + t * direction;
        }
        const point3 origin;
        const vec3 direction;
        const color attenuation;
        const int index;
        const int depth;
    };


}

#endif // RAY_H