
#ifndef BVH_H
#define BVH_H

#include <functional>
#include <stack>
#include "common.h"
#include "geometry/ray.h"
#include "geometry/interval.h"





namespace geom {
    using BVHView = View<geom::Bounds>;

    struct BVHNode {
        int leftChild{ -1 };
        int rightChild{ -1 };
        Bounds bounds;
        Entity entity{ INVALID };
    };


    class BVHSystem :public System {
    public:
        bool hit(const AABB& box, const render::Ray& r, Interval ray_t) {
            for (int dim = 0; dim < 3; ++dim) {
                const double t0 = box[dim].min - r.origin[dim] / r.direction[dim];
                const double t1 = box[dim].max - r.origin[dim] / r.direction[dim];
                if (t0 < t1) {
                    if (t0 > ray_t.min) ray_t.min = t0;
                    if (t1 < ray_t.max) ray_t.max = t1;
                }
                else {
                    if (t1 > ray_t.min) ray_t.min = t1;
                    if (t0 < ray_t.max) ray_t.max = t0;
                }

                if (ray_t.max <= ray_t.min)
                    return false;
            }
            return true;
        }

        void build(const BVHView& view, RNG& rng) {
            std::vector<std::tuple<Entity, std::reference_wrapper<geom::Bounds>>> leaves;
            for (const auto elem : view) {
                leaves.push_back(elem);
            }
            if (leaves.size() == 0) {
                return;
            }
            std::stack<std::tuple<int, int, int>> ranges;
            ranges.emplace(0, leaves.size(), -1);
            while (!ranges.empty()) {
                const auto [start, end, parent] = ranges.top();
                ranges.pop();
                BVHNode next_node;
                auto envelope = std::get<1>(leaves[start]).get();
                for (int i = start + 1; i < end; i++) {
                    envelope = Bounds(envelope, std::get<1>(leaves[i]).get());
                }
                next_node.bounds = envelope;
                const int node_index = nodes.size();
                if (parent != -1) {
                    nodes[parent].rightChild = node_index;
                }
                if (end - start == 1) {
                    next_node.entity = std::get<Entity>(leaves[start]);
                }
                else {
                    next_node.leftChild = nodes.size() + 1;
                }
                nodes.push_back(std::move(next_node));
                if (end - start == 1) {
                    continue;
                }

                int sort_dimension = rng.random_int(0, 2);
                int median = (start + end) / 2;
                std::nth_element(
                    leaves.begin() + start,
                    leaves.begin() + end,
                    leaves.begin() + median,
                    [sort_dimension](const auto& a, const auto& b)
                    {
                        return
                            std::get<1>(a).get().world_bounds[sort_dimension].midpoint() <
                            std::get<1>(b).get().world_bounds[sort_dimension].midpoint();
                    }
                );
                ranges.emplace(median, end, node_index);
                ranges.emplace(start, median, -1);
            }
        }


    private:
        std::vector<BVHNode> nodes;
    };


}
#endif

