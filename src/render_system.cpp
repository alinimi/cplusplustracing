#include "common.h"
#include <iostream>
#include "render_system.h"
#include "camera.h"
#include "geometry/interval.h"

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

	std::optional<color> RenderSystem::scatter(const Ray& r, const HitRecord& hit) {

	}


	std::optional<HitRecord> RenderSystem::hit(ECS& ecs, const Ray& r, Interval ray_t) {
		std::optional<HitRecord> closest_hit;
		auto closest_so_far = ray_t.max;
		for (auto const& entity : entities)
		{
			auto& sphere = ecs.getComponent<Sphere>(entity);
			auto hit = hit_sphere(sphere, r, Interval(ray_t.min, closest_so_far));
			if (hit.has_value() && hit->t < closest_so_far) {
				closest_hit = hit;
				closest_so_far = hit->t;
			}
		}
		return closest_hit;

	}



	color RenderSystem::ray_color(ECS& ecs, const Ray& r, int depth) {
		if (depth < 0) {
			return color(0., 0., 0.);
		}
		std::optional<HitRecord> closest_hit = hit(ecs, r, Interval(0, infinity));
		if (closest_hit.has_value()) {
			vec3 direction = closest_hit->normal + random_unit_vector();
			return 0.5 * ray_color(ecs, Ray(closest_hit->p + 1e-8 * closest_hit->normal, direction), depth - 1);
		}

		auto a = 0.5 * (glm::normalize(r.direction).y + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}


	std::vector<float> RenderSystem::render(ECS& ecs, const Camera& cam) {
		std::vector<float> image(cam.width * cam.height * m_channels);

		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {
				color pixel_color = color(0., 0., 0.);
				for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
					Ray r = cam.get_ray(x, y);

					pixel_color += ray_color(ecs, r, cam.max_depth);
				}
				pixel_color /= cam.samples_per_pixel;
				Interval intensity(0., 1.);
				int idx = (y * cam.width + x) * m_channels;
				image[idx + 0] = intensity.clamp(pixel_color.x);   // R
				image[idx + 1] = intensity.clamp(pixel_color.y);   // G
				image[idx + 2] = intensity.clamp(pixel_color.z);   // B

			}
		}return image;
	}






	std::vector<float> RenderSystem::render_ecs(ECS& ecs, const Camera& cam) {
		std::vector<color> pixel_colors(cam.width * cam.height, color(0., 0., 0.));

		std::queue<Ray> rays;

		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {
				for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
					Ray r = cam.get_ray(x, y);
					int idx = (y * cam.width + x);
					r.index = idx;
					r.depth = cam.max_depth;
					r.attenuation = color(1., 1., 1.);
					rays.push(r);
				}
			}
		}
		while (!rays.empty()) {
			Ray r = rays.front();
			rays.pop();
			if (r.depth < 0) {
				continue;
			}
			std::optional<HitRecord> closest_hit = hit(ecs, r, Interval(0, infinity));
			if (closest_hit.has_value()) {
				vec3 direction = closest_hit->normal + random_unit_vector();

				Ray new_ray(closest_hit->p + 1e-8 * closest_hit->normal, direction);
				new_ray.depth = r.depth - 1;
				new_ray.index = r.index;
				new_ray.attenuation = r.attenuation * 0.5;
				rays.push(new_ray);
			}
			else {
				auto a = 0.5 * (glm::normalize(r.direction).y + 1.0);
				color background_color = (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
				pixel_colors[r.index] += background_color * r.attenuation;
			}
		}

		std::vector<float> image(cam.width * cam.height * m_channels);
		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {

				color pixel_color = pixel_colors[y * cam.width + x] / double(cam.samples_per_pixel);
				Interval intensity(0., 1.);
				int idx = (y * cam.width + x) * m_channels;

				image[idx + 0] = intensity.clamp(pixel_color.x);   // R
				image[idx + 1] = intensity.clamp(pixel_color.y);   // G
				image[idx + 2] = intensity.clamp(pixel_color.z);   // B

			}
		}
		return image;
	}

}