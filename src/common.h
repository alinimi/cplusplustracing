
#ifndef COMMON_H
#define COMMON_H
#define GLM_ENABLE_EXPERIMENTAL
#include <limits>
#include <glm/gtx/norm.hpp>
#include <algorithm>
#include <functional>
#include <random>
#include <glm/glm.hpp>
using point3 = glm::dvec3;
using color = glm::dvec3;
using vec3 = glm::dvec3;

constexpr double infinity = std::numeric_limits<double>::infinity();

struct RNG {
    RNG() : generator() {}
    RNG(uint_fast32_t seed) :generator(seed) {}

    RNG clone() {

        uint_fast32_t new_seed = uint_distribution(generator);
        return RNG(new_seed);
        // return RNG(uint_distribution(generator));
    }

    double random_double() {
        return double_distribution(generator);
    }
    double random_double(double min, double max) {
        return min + (max - min) * random_double();

    }
    int random_int(int min, int max) {
        return std::uniform_int_distribution<int>(min,max)(generator);
    }

    std::mt19937 generator;
    std::uniform_real_distribution<double> double_distribution{ 0.0, 1.0 };
    std::uniform_int_distribution<uint_fast32_t> uint_distribution{
        std::numeric_limits<uint_fast32_t>::min(),
        std::numeric_limits<uint_fast32_t>::max()
    };
};


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