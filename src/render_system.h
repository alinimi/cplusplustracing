#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <optional>
#include <indicators/progress_bar.hpp>
#include "camera.h"
#include "ecs/ECS.h"
#include "geometry/hittable.h"
#include "geometry/hit_record.h"
#include "geometry/interval.h"

using namespace indicators;

namespace render {
    class RenderSystem :public System {
    public:
        std::optional<HitRecord> hit_sphere(const Sphere& sphere, const Ray& r, Interval ray_t) const;
        std::optional<HitRecord> hit(ECS& ecs, const Ray& r, Interval ray_t) const;
        std::optional<Ray> scatter_lambertian(const Material& mat, const Ray& r, const HitRecord& rec, RNG& rng) const;
        std::optional<Ray> scatter_metallic(const Material& mat, const Ray& r, const HitRecord& rec, RNG& rng) const;
        std::optional<Ray> scatter_dielectric(const Material& mat, const Ray& r, const HitRecord& rec, RNG& rng) const;
        std::optional<Ray> scatter(ECS& ecs, const Ray& r, const HitRecord& rec, RNG& rng) const;
        void render_pixel(ECS& ecs, const Camera& cam, int x, int y, std::vector<color>& pixel_colors, RNG& rng) const;
        void render_tile(
            int i0, int i1, int j0, int j1,
            ECS& ecs, const Camera& cam,
            std::vector<color>& pixel_colors,
            std::atomic<int>& finished_blocks,
            ProgressBar& bar,
            int total_blocks,
            RNG rng
        ) const;
        std::vector<float> render_ecs(ECS& ecs, const Camera& cam, RNG& rng) const;

        View<render::Sphere,render::Material> view;

    private:
        int m_channels = 3; // Number of color channels (R, G, B)
    };

}
#endif // RENDER_SYSTEM_H