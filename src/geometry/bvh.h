#ifndef BVH_H
#define BVH_H

#include <stack>
#include "bounds.h"
#include "interval.h"
#include "ray.h"
#include "../common.h"


namespace geom {
    struct BVHNode {
        int leftChild{ -1 };
        int rightChild{ -1 };
        Bounds bounds;
        Entity entity{ INVALID };
    };


    class BVH {
        friend class BVHSystem;
    public:
        std::vector<Entity> intersect(const render::Ray& r, geom::Interval ray_t) const {
            std::vector<Entity> candidates;
            assert(nodes.size() != 0 && "Empty BVH");
            std::stack<size_t> nodes_to_test;
            nodes_to_test.push(0);
            if (!hit_bounds(nodes[0].bounds.world_bounds, r, ray_t)) {
                return {};
            }
            while (!nodes_to_test.empty()) {
                auto current_node = nodes[nodes_to_test.top()];
                nodes_to_test.pop();
                if (current_node.entity != INVALID) {
                    candidates.push_back(current_node.entity);
                }
                else if (current_node.rightChild != -1 && hit_bounds(nodes[current_node.rightChild].bounds.world_bounds, r, ray_t)) {
                    nodes_to_test.push(current_node.rightChild);
                }
                if (current_node.leftChild != -1 && hit_bounds(nodes[current_node.leftChild].bounds.world_bounds, r, ray_t)) {
                    nodes_to_test.push(current_node.leftChild);
                }
            }
            return candidates;
        }

    private:
        bool hit_bounds(const AABB& box, const render::Ray& r, Interval ray_t) const {
            for (int dim = 0; dim < 3; ++dim) {
                const double t0 = (box[dim].min - r.origin[dim]) / r.direction[dim];
                const double t1 = (box[dim].max - r.origin[dim]) / r.direction[dim];
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
        std::vector<BVHNode> nodes;
    };


}

#endif

