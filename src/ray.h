# ifndef RAY_H
# define RAY_H
#include <glm/glm.hpp>
using point3 = glm::dvec3;
using color = glm::dvec3;
using vec3 = glm::dvec3;


class Ray {
public:
    Ray() = default; // Default constructor
    Ray(const vec3& origin, const vec3& direction)
        : origin(origin), direction(direction) {
    }

    const vec3& getOrigin() const {
        return origin;
    }

    const vec3& getDirection() const {
        return direction;
    }

private:
    vec3 origin;  // Ray origin
    vec3 direction; // Ray direction
};
#endif // RAY_H