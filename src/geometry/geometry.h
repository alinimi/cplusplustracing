#include "../common.h"


namespace geom {
    constexpr inline double degrees_to_radians(double degrees) {
        return degrees * M_PI / 180.;
    }

    inline vec3 random_vec3(RNG& rng) {
        return vec3(rng.random_double(), rng.random_double(), rng.random_double());
    }

    inline vec3 random_vec3(double min, double max, RNG& rng) {
        return vec3(rng.random_double(min, max), rng.random_double(min, max), rng.random_double(min, max));
    }

    inline vec3 random_unit_vector(RNG& rng) {
        auto p = random_vec3(-1., 1., rng);
        while (glm::length2(p) > 1 || glm::length2(p) <= 1e-160) {
            p = random_vec3(-1., 1., rng);
        }
        return p / glm::length(p);
    }

    inline vec3 random_in_unit_disk(RNG& rng) {
        while (true) {
            auto p = vec3(rng.random_double(-1, 1), rng.random_double(-1, 1), 0);
            if (glm::length2(p) < 1)
                return p;
        }
    }

    inline vec3 random_on_hemisphere(vec3 n, RNG& rng) {
        vec3 unit_vector = random_unit_vector(rng);
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

}