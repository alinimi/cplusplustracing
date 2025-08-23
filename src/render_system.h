#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <optional>
#include "camera.h"
#include "ecs/ECS.h"
#include "geometry/hittable.h"
#include "geometry/hit_record.h"
#include "geometry/interval.h"

namespace render {




    class RenderSystem :public System {
    public:
        std::optional<HitRecord> hit_sphere(const Sphere& sphere, const Ray& r, Interval ray_t);
        std::optional<HitRecord> hit(ECS& ecs, const Ray& r, Interval ray_t);
        std::optional<color> scatter(const Ray& r, const HitRecord& hit);
        color ray_color(ECS& ecs, const Ray& r, int depth);
        std::vector<float> render(ECS& ecs, const Camera& cam);
        std::vector<float> render_ecs(ECS& ecs, const Camera& cam);

    private:
        int m_channels = 3; // Number of color channels (R, G, B)


    };

}
#endif // RENDER_SYSTEM_H