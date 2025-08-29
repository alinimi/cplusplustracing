#include "common.h"
#include <iostream>
#include "ecs/entity.h"
#include "render_system.h"
#include "camera.h"
#include "geometry/interval.h"
#include "material/material.h"

namespace render {

	std::optional<HitRecord> RenderSystem::hit_sphere(const Sphere& sphere, const Ray& r, Interval ray_t) const {
		const vec3 oc = sphere.center - r.origin;
		const auto a = glm::length2(r.direction);
		const auto h = glm::dot(r.direction, oc);
		const auto c = glm::length2(oc) - sphere.radius * sphere.radius;
		const auto discriminant = h * h - a * c;


		if (discriminant < 0) {
			return {};
		}

		const auto sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root))
				return {};
		}

		const auto rec_t = root;
		const auto point = r.at(rec_t);

		return HitRecord{ rec_t,point,sphere.normal(point),r };
	}

	std::optional<Ray> RenderSystem::scatter_lambertian(const Material& mat, const Ray& r, const HitRecord& rec) const {
		auto scatter_direction = rec.normal + random_unit_vector();
		if (near_zero(scatter_direction)) {
			scatter_direction = rec.normal;
		}
		return r.scattered(offset(rec.p, scatter_direction, 1e-3), scatter_direction, r.attenuation * mat.albedo);
	}

	std::optional<Ray> RenderSystem::scatter_metallic(const Material& mat, const Ray& r, const HitRecord& rec) const {
		vec3 reflected = reflect(r.direction, rec.normal);
		reflected = glm::normalize(reflected) + (mat.fuzz * random_unit_vector());
		if (glm::dot(reflected, rec.normal) < 0) {
			return {};
		}
		return r.scattered(offset(rec.p, reflected, 1e-3), reflected, r.attenuation * mat.albedo);
	}

	std::optional<Ray> RenderSystem::scatter_dielectric(const Material& mat, const Ray& r, const HitRecord& rec) const {
		double cos_theta = std::fmin(glm::dot(-glm::normalize(r.direction), rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
		const double ri = rec.front_face ? (1.0 / mat.refraction_index) : mat.refraction_index;
		vec3 direction;
		vec3 p;
		if (ri * sin_theta > 1. || reflectance(cos_theta, ri) > random_double()) {
			direction = reflect(r.direction, rec.normal);
			p = offset(rec.p, direction, 1e-3);
		}
		else {
			direction = refract(glm::normalize(r.direction), rec.normal, ri);
			p = offset(rec.p, direction, 1e-3);//rec.p - rec.normal * 1e-5;
		}

		return r.scattered(p, direction, r.attenuation);//Ray(p,direction,r.attenuation,r.index,0);
	}

	std::optional<Ray> RenderSystem::scatter(ECS& ecs, const Ray& r, const HitRecord& rec) const {
		const double t = random_double();
		const double s = random_double();
		// TODO: Probably expensive to search for the associated material at each hit,
		// allow sphere to have material directly?
		const auto& mat = ecs.getComponent<Material>(rec.entity);
		if (t > mat.metallic) {
			if (s > mat.dielectric) {
				return scatter_lambertian(mat, r, rec);
			}
			return scatter_dielectric(mat, r, rec);

		}
		return scatter_metallic(mat, r, rec);
	}


	std::optional<HitRecord> RenderSystem::hit(ECS& ecs, const Ray& r, Interval ray_t) const {
		std::optional<HitRecord> closest_hit;
		auto closest_so_far = ray_t.max;
		for (auto entity : entities)
		{
			const auto& sphere = ecs.getComponent<Sphere>(entity);
			const auto hit = hit_sphere(sphere, r, Interval(ray_t.min, closest_so_far));
			if (hit.has_value() && hit->t < closest_so_far) {
				closest_hit = hit;
				closest_hit->entity = entity;
				closest_so_far = hit->t;
			}
		}
		return closest_hit;

	}


	std::vector<float> RenderSystem::render_ecs(ECS& ecs, const Camera& cam) const {
		std::vector<color> pixel_colors(cam.width * cam.height, color(0., 0., 0.));

		std::queue<Ray> rays;

		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {
				for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
					Ray r = cam.get_ray(x, y);
					while (true) {
						if (r.depth < 0) {
							break;
						}
						const std::optional<HitRecord> closest_hit = hit(ecs, r, Interval(0, infinity));
						if (closest_hit.has_value()) {
							const vec3 direction = closest_hit->normal + random_unit_vector();
							const auto new_ray = scatter(ecs, r, closest_hit.value());
							if (new_ray.has_value()) {
								r = new_ray.value();
							}
							else {
								break;
							}
						}
						else {
							auto a = 0.5 * (glm::normalize(r.direction).y + 1.0);
							const color background_color = (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
							pixel_colors[r.index] += background_color * r.attenuation;
							break;
						}
					}
				}
			}
		}

		std::vector<float> image(cam.width * cam.height * m_channels);
		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {

				const color pixel_color = pixel_colors[y * cam.width + x] / double(cam.samples_per_pixel);
				const Interval intensity(0., 1.);
				const int idx = (y * cam.width + x) * m_channels;
				//TODO: Avoid this copy, use span?
				image[idx + 0] = intensity.clamp(pixel_color.x);   // R
				image[idx + 1] = intensity.clamp(pixel_color.y);   // G
				image[idx + 2] = intensity.clamp(pixel_color.z);   // B

			}
		}
		return image;
	}

}