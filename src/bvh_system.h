
#ifndef BVH_SYSTEM_H
#define BVH_SYSTEM_H

#include <functional>
#include <stack>
#include "common.h"

namespace geom {
    class BVHSystem :public System {
    public:

        template<typename View>
        geom::BVH build(const View& view, RNG& rng) {
            geom::BVH bvh;
            std::vector<std::tuple<Entity, std::reference_wrapper<geom::Bounds>>> leaves;
            for (const auto elem : view) {
                leaves.emplace_back(std::get<Entity>(elem),std::get<geom::Bounds&>(elem));
            }
            assert(leaves.size() != 0 && "Empty scene");
            std::stack<std::tuple<int, int, int>> ranges;
            ranges.emplace(0, leaves.size(), -1);
            while (!ranges.empty()) {
                const auto [start, end, parent] = ranges.top();
                ranges.pop();
                geom::BVHNode next_node;
                auto envelope = std::get<1>(leaves[start]).get();
                for (int i = start + 1; i < end; i++) {
                    envelope = geom::Bounds(envelope, std::get<1>(leaves[i]).get());
                }
                next_node.bounds = envelope;
                const int node_index = bvh.nodes.size();
                if (parent != -1) {
                    bvh.nodes[parent].rightChild = node_index;
                }
                if (end - start == 1) {
                    next_node.entity = std::get<Entity>(leaves[start]);
                }
                else {
                    next_node.leftChild = bvh.nodes.size() + 1;
                }
                bvh.nodes.push_back(std::move(next_node));
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
            return bvh;
        }

    };
}

#endif

