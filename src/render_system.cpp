#include "common.h"
#include <iostream>
#include <thread>
#include <tuple>
#include "ecs/entity.h"
#include "render_system.h"
#include "camera.h"
#include "geometry/interval.h"
#include "material/material.h"

namespace render {

	std::optional<HitRecord> RenderSystem::hit_sphere(const geom::Sphere& sphere, const Ray& r, geom::Interval ray_t) const {
		const vec3 current_center = sphere.center + sphere.direction * r.time;
		const vec3 oc = current_center - r.origin;
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

		return HitRecord{ rec_t,point,(point - current_center) / sphere.radius,r };
	}

	std::optional<Ray> RenderSystem::scatter_lambertian(const Material& mat, const Ray& r, const HitRecord& rec, RNG& rng) const {
		auto scatter_direction = rec.normal + geom::random_unit_vector(rng);
		if (geom::near_zero(scatter_direction)) {
			scatter_direction = rec.normal;
		}
		return r.scattered(geom::offset(rec.p, scatter_direction, 1e-3), scatter_direction, r.attenuation * mat.albedo);
	}

	std::optional<Ray> RenderSystem::scatter_metallic(const Material& mat, const Ray& r, const HitRecord& rec, RNG& rng) const {
		vec3 reflected = reflect(r.direction, rec.normal);
		reflected = glm::normalize(reflected) + (mat.fuzz * geom::random_unit_vector(rng));
		if (glm::dot(reflected, rec.normal) < 0) {
			return {};
		}
		return r.scattered(geom::offset(rec.p, reflected, 1e-3), reflected, r.attenuation * mat.albedo);
	}

	std::optional<Ray> RenderSystem::scatter_dielectric(const Material& mat, const Ray& r, const HitRecord& rec, RNG& rng) const {
		double cos_theta = std::fmin(glm::dot(-glm::normalize(r.direction), rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
		const double ri = rec.front_face ? (1.0 / mat.refraction_index) : mat.refraction_index;
		vec3 direction;
		vec3 p;
		if (ri * sin_theta > 1. || geom::reflectance(cos_theta, ri) > rng.random_double()) {
			direction = geom::reflect(r.direction, rec.normal);
			p = geom::offset(rec.p, direction, 1e-3);
		}
		else {
			direction = geom::refract(glm::normalize(r.direction), rec.normal, ri);
			p = geom::offset(rec.p, direction, 1e-3);//rec.p - rec.normal * 1e-5;
		}
		return r.scattered(p, direction, r.attenuation);//Ray(p,direction,r.attenuation,r.index,0);
	}

	std::optional<Ray> RenderSystem::scatter(const RenderView& view, const Ray& r, const HitRecord& rec, RNG& rng) const {
		const double t = rng.random_double();
		const double s = rng.random_double();
		if (t > rec.mat.metallic) {
			if (s > rec.mat.dielectric) {
				return scatter_lambertian(rec.mat, r, rec, rng);
			}
			return scatter_dielectric(rec.mat, r, rec, rng);
		}
		return scatter_metallic(rec.mat, r, rec, rng);
	}


	std::optional<HitRecord> RenderSystem::hit(const RenderView& view, const Ray& r, geom::Interval ray_t) const {
		std::optional<HitRecord> closest_hit;
		auto closest_so_far = ray_t.max;
		for (const auto [e, sphere, material] : view)
		{
			const auto hit = hit_sphere(sphere, r, geom::Interval(ray_t.min, closest_so_far));
			if (hit.has_value() && hit->t < closest_so_far) {
				closest_hit = hit;
				closest_hit->mat = material;
				closest_so_far = hit->t;
			}
		}
		return closest_hit;

	}



	void RenderSystem::render_pixel(const RenderView& view, const Camera& cam, int x, int y, std::vector<color>& pixel_colors, RNG& rng) const {
		for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
			Ray r = cam.get_ray(x, y, rng);
			while (true) {
				if (r.depth < 0) {
					break;
				}
				const std::optional<HitRecord> closest_hit = hit(view, r, geom::Interval(0, infinity));
				if (closest_hit.has_value()) {
					const vec3 direction = closest_hit->normal + geom::random_unit_vector(rng);
					const auto new_ray = scatter(view, r, closest_hit.value(), rng);
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


	void RenderSystem::render_tile(int i0, int i1, int j0, int j1,
		const RenderView& view,
		const Camera& cam,
		std::vector<color>& pixel_colors,
		std::atomic<int>& finished_blocklines,
		ProgressBar& bar,
		int total_blocklines,
		RNG thread_rng
	) const {
		for (int j = j0; j < j1; ++j) {
			for (int i = i0; i < i1; ++i) {
				for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
					Ray r = cam.get_ray(i, j, thread_rng);
					while (true) {
						if (r.depth < 0) {
							break;
						}
						const std::optional<HitRecord> closest_hit = hit(view, r, geom::Interval(0, infinity));
						if (closest_hit.has_value()) {
							const vec3 direction = closest_hit->normal + geom::random_unit_vector(thread_rng);
							const auto new_ray = scatter(view, r, closest_hit.value(), thread_rng);
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
			finished_blocklines++;
			bar.set_progress(std::floor((float(finished_blocklines) / float(total_blocklines)) * 100.f));
		}

	}

	std::vector<float> RenderSystem::render_ecs(const RenderView& view, const Camera& cam, RNG& rng) const {
		std::vector<color> pixel_colors(cam.width * cam.height, color(0., 0., 0.));

		ProgressBar bar{
			option::BarWidth{50},
			option::Start{"["},
			option::Fill{"="},
			option::Lead{">"},
			option::Remainder{" "},
			option::End{"]"},
			option::PostfixText{"Render"},
			option::ForegroundColor{Color::green},
			option::ShowPercentage{true},
			option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
		};
		std::vector<std::thread> threads;

		const int block_width = cam.width;
		const int block_height = std::ceil(cam.height);

		const int total_blocklines = std::ceil(cam.width / float(block_width)) * cam.height;
		const int total_blocks = std::ceil(cam.width / float(block_width)) * std::ceil(cam.height / float(block_height));
		std::atomic<int> finished_blocklines = 0;

		for2dTiled(cam.width, cam.height, block_height, block_width,
			[&](int i0, int i1, int j0, int j1) {
				threads.push_back(std::thread(
					&RenderSystem::render_tile,
					this,
					i0, i1, j0, j1,
					std::cref(view),
					std::cref(cam),
					std::ref(pixel_colors),
					std::ref(finished_blocklines),
					std::ref(bar),
					total_blocklines,
					rng.clone()));
			});
		for (auto& thread : threads) {
			thread.join();
		}


		std::vector<float> image(cam.width * cam.height * m_channels);
		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {

				const color pixel_color = pixel_colors[y * cam.width + x] / double(cam.samples_per_pixel);
				const geom::Interval intensity(0., 1.);
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