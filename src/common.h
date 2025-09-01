
#ifndef COMMON_H
#define COMMON_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <algorithm>
#include <functional>
#include <random>
#include <glm/glm.hpp>
using point3 = glm::dvec3;
using color = glm::dvec3;
using vec3 = glm::dvec3;

constexpr double infinity = std::numeric_limits<double>::infinity();

constexpr inline double degrees_to_radians(double degrees){
    return degrees * M_PI / 180.;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator(3);
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
    return v - 2 * glm::dot(v, n) * n;
}

inline vec3 refract(const vec3 uv, const vec3 n, double etai_over_etat) {
    auto cos_theta = std::fmin(glm::dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - glm::length2(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

inline vec3 offset(vec3 p, vec3 dir, double m) {
    return p + dir * m;
}


static double reflectance(double cosine, double refraction_index) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

inline bool near_zero(vec3 n) {
    auto s = 1e-8;
    return (std::fabs(n.x) < s) && (std::fabs(n.y) < s) && (std::fabs(n.z) < s);
}



inline void for2dTiled(int N, int M, int tileH, int tileW,
    std::function<void(int, int, int, int)> func) {
    for (int i = 0; i < N; i += tileW) {
        for (int j = 0; j < M; j += tileH) {
            int iEnd = std::min(i + tileW, N);
            int jEnd = std::min(j + tileH, M);
            func(i, iEnd, j, jEnd);
        }
    }
}

#endif // COMMON_H