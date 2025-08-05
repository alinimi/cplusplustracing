#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include "render_system.h"
#include "camera.h"
#include <iostream>

namespace render {

	std::optional<HitRecord> RenderSystem::hit_sphere(const Sphere& sphere, const Ray& r, Interval ray_t) {
		vec3 oc = sphere.center - r.origin;
		auto a = glm::length2(r.direction);
		auto h = glm::dot(r.direction, oc);
		auto c = glm::length2(oc) - sphere.radius * sphere.radius;
		auto discriminant = h * h - a * c;


		if (discriminant < 0) {
			return {};
		}

		auto sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root))
				return {};
		}

		auto rec_t = root;
		auto point = r.at(rec_t);

		return HitRecord{ rec_t,point,sphere.normal(point),r };
	}



	color RenderSystem::ray_color(ECS& ecs, const Ray& r, Interval ray_t) {
		std::optional<HitRecord> closest_hit;
		auto closest_so_far = ray_t.max;
		for (auto const& entity : entities)
		{
			auto& sphere = ecs.getComponent<Sphere>(entity);
			auto hit = hit_sphere(sphere, r, Interval(ray_t.min,closest_so_far));
			if (hit.has_value() && hit->t < closest_so_far) {
				closest_hit = hit;
				closest_so_far = hit->t;
			}
		}
		if(closest_hit.has_value()){
			return(0.5 * (closest_hit->normal + color(1., 1., 1.)));
		}

		auto a = 0.5 * (glm::normalize(r.direction).y + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}


	std::vector<float> RenderSystem::render(ECS& ecs, const Camera& cam) {
		std::vector<float> image(cam.width * cam.height * m_channels);



		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {
				Ray r = cam.camera_ray(x, y);

				int idx = (y * cam.width + x) * m_channels;
				color pixel_color = ray_color(ecs, r, Interval(0., infinity));
				image[idx + 0] = pixel_color.x;   // R
				image[idx+ 1] = pixel_color.y;   // G
				image[idx + 2] = pixel_color.z;   // B
				int ir = int(255.999 * pixel_color.x);
				int ig = int(255.999 * pixel_color.y);
				int ib = int(255.999 * pixel_color.z);

			}
		}return image;
	}
}