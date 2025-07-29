#ifndef INTERVAL_H
#define INTERVAL_H

#include "../common.h"

namespace render {



    struct Interval {
        const double min, max;

        constexpr Interval() : min(-infinity), max(infinity) {};
        constexpr Interval(double min, double max) : min(min), max(max) {};
        constexpr double size() const {
            return max - min;
        }
        constexpr bool contains(double x) const {
            return x <= max && x >= min;
        }
        constexpr bool surrounds(double x) const {
            return x<max && x >min;
        }
        static const Interval empty, universe;

    };
    inline constexpr Interval Interval::empty = Interval(+infinity, -infinity);
    inline constexpr Interval Interval::universe = Interval(-infinity, +infinity);

}
#endif //INTERVAL_H
