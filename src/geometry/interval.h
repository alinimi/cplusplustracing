#ifndef INTERVAL_H
#define INTERVAL_H


#include <algorithm>
#include "../common.h"

namespace geom {



    struct Interval {

        constexpr Interval() : min(-infinity), max(infinity) {};
        constexpr Interval(double min, double max) : min(min), max(max) {};
        constexpr Interval(const Interval& a, const Interval& b) :
            min(a.min <= b.min ? a.min : b.min), max(a.max >= b.max ? a.max : b.max)
        {
            // Create the interval tightly enclosing the two input intervals.
            // min = a.min <= b.min ? a.min : b.min;
            // max = a.max >= b.max ? a.max : b.max;
        }

        constexpr double size() const {
            return max - min;
        }
        constexpr double midpoint() const {
            return (max + min) / 2.;
        }
        constexpr bool contains(double x) const {
            return x <= max && x >= min;
        }
        constexpr bool surrounds(double x) const {
            return x<max && x >min;
        }
        constexpr double clamp(double x) const {
            return std::clamp<double>(x, min, max);
        }
        constexpr Interval expand(double delta) const {
            return Interval(min - delta / 2., max + delta / 2.);
        }
        double min, max;

        static const Interval empty, universe;

    };
    inline constexpr Interval Interval::empty = Interval(+infinity, -infinity);
    inline constexpr Interval Interval::universe = Interval(-infinity, +infinity);

}
#endif //INTERVAL_H
