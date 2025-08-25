
#ifndef COMMON_H
#define COMMON_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <random>
#include <glm/glm.hpp>
using point3 = glm::dvec3;
using color = glm::dvec3;
using vec3 = glm::dvec3;

constexpr double infinity = std::numeric_limits<double>::infinity();


inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline vec3 random_vec3() {
    return vec3(random_double(), random_double(), random_double());
}

inline vec3 random_vec3(double min, double max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline vec3 random_unit_vector() {
    auto p = random_vec3(-1., 1.);
    while (glm::length2(p) > 1 || glm::length2(p) <= 1e-160) {
        p = random_vec3(-1., 1.);
    }
    return p / glm::length(p);
}

inline vec3 random_on_hemisphere(vec3 n) {
    vec3 unit_vector = random_unit_vector();
    if (glm::dot(unit_vector, n) < 0) {
        return -unit_vector;
    }
    return unit_vector;
}

inline vec3 reflect(const vec3 v, const vec3 n) {
    return v - 2 * dot(v, n) * n;
}

inline bool near_zero(vec3 n) {
    auto s = 1e-8;
    return (std::fabs(n.x) < s) && (std::fabs(n.y) < s) && (std::fabs(n.z) < s);
}

#endif // COMMON_H