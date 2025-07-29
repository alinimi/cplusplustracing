#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include "render_system.h"
#include "camera.h"
#include <iostream>

namespace render {

	std::optional<HitRecord> RenderSystem::hit_sphere(const Sphere& sphere, const Ray& r, double ray_tmin, double ray_tmax) {

		vec3 oc = sphere.center - r.origin();
		auto a = glm::length2(r.direction());
		auto h = glm::dot(r.direction(), oc);
		auto c = glm::length2(oc) - sphere.radius * sphere.radius;
		auto discriminant = h * h - a * c;

		if (discriminant < 0) {
			return {};
		}
		auto sqrtd = std::sqrt(discriminant);


		// Find the nearest root that lies in the acceptable range.
		auto root = (h - sqrtd) / a;
		if (root <= ray_tmin || ray_tmax <= root) {
			root = (h + sqrtd) / a;
			if (root <= ray_tmin || ray_tmax <= root)
				return {};
		}

		auto rec_t = root;
		auto point = r.at(rec_t);

		return HitRecord{ rec_t,point,sphere.normal(point),r };
	}



	color RenderSystem::ray_color(ECS& ecs, const Ray& r, double ray_tmin, double ray_tmax) {
		for (auto const& entity : entities)
		{
			// std::cout << "processing entity" << std::endl;
			auto& sphere = ecs.getComponent<Sphere>(entity);
			auto hit = hit_sphere(sphere, r, ray_tmin, ray_tmax);
			if (hit.has_value()) {
				return(0.5 * (glm::normalize(r.at(hit->t) - sphere.center) + color(1., 1., 1.)));
				// return { 0.5,0.0,0.0 };
			}

		}
		auto a = 0.5 * (glm::normalize(r.direction()).y + 1.0);
		// return color(0.0,0.0,a);
		// return glm::normalize(r.direction());
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}


	std::vector<float> RenderSystem::render(ECS& ecs, const Camera& cam) {
		std::vector<float> image(cam.width * cam.height * m_channels);



		// std::cout << "P3\n" << cam.width << ' ' << cam.height << "\n255\n";
		for (int y = 0; y < cam.height; ++y) {
			for (int x = 0; x < cam.width; ++x) {
				Ray r = cam.camera_ray(x, y);
				// std::cout << "x " << x << " y " << y << " " << r.direction().x << " " << r.direction().y << " " << r.direction().z<< std::endl;

				int idx = (y * cam.width + x) * m_channels;
				color pixel_color = ray_color(ecs, r, 0., std::numeric_limits<double>::infinity());
				image[idx + 0] = pixel_color.x;   // R
				image[idx+ 1] = pixel_color.y;   // G
				image[idx + 2] = pixel_color.z;   // B
				// for (auto const& entity : entities)
				// {
				// 	auto& sphere = ecs.getComponent<Sphere>(entity);

				// }


				int ir = int(255.999 * pixel_color.x);
				int ig = int(255.999 * pixel_color.y);
				int ib = int(255.999 * pixel_color.z);

				// std::cout << ir << ' ' << ig << ' ' << ib << '\n';
			}
		}return image;

	}

	// std::vector<float> image(cam.width * cam.height * channels);
	// for (int y = 0; y < cam.height; ++y) {
	//     std::cout << "Scanlines remaining " << (cam.height - y) << std::endl;
	//     for (int x = 0; x < cam.width; ++x) {
	//         Ray r = cam.camera_ray(x, y);
	//         int idx = (y * cam.width + x) * channels;
	//         color pixel_color = ray_color(r);
	//         image[idx + 0] = pixel_color.x;   // R
	//         image[idx + 1] = pixel_color.y;   // G
	//         image[idx + 2] = pixel_color.z;   // B
	//     }
	// }

}