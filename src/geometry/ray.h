#ifndef RAY_H
#define RAY_H


#include "../common.h"


namespace render {



    class Ray {
    public:
        constexpr Ray(
            point3 origin,
            vec3 direction,
            color attenuation,
            int index,
            int depth
        ) :
            origin(origin), direction(direction), attenuation(attenuation), index(index), depth(depth)
        {
        }

        constexpr Ray scattered(
            point3 new_origin,
            vec3 new_direction,
            color new_attenuation
        ) const
        {
            return Ray(new_origin, new_direction, new_attenuation, index, depth - 1);
        }

        constexpr point3 at(double t) const {
            return origin + t * direction;
        }
        point3 origin;
        vec3 direction;
        color attenuation;
        int index;
        int depth;
    };


}

#endif // RAY_H