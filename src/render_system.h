#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <optional>
#include "camera.h"
#include "ecs/ECS.h"
#include "hittable.h"
namespace render {




    class RenderSystem :public System {
    public:
        color ray_color(ECS& ecs, const Ray& r, double ray_tmin, double ray_tmax) ;
        std::optional<HitRecord> hit_sphere(const Sphere& sphere, const Ray& r,double ray_tmin, double ray_tmax);
        std::vector<float> render(ECS& ecs, const Camera& cam);

    private:
        int m_channels = 3; // Number of color channels (R, G, B)
    };


}
#endif // RENDER_SYSTEM_H