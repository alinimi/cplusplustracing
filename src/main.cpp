#include <chrono>
#include <iostream>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <glm/glm.hpp>
#include "ecs/ECS.h"
#include "geometry/bounds.h"
#include "geometry/hittable.h"
#include "geometry/ray.h"
#include "material/material.h"
#include "render_system.h"
#include "bounds_update_system.h"
#include "bvh_system.h"

render::Camera create_camera() {
    // image params
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 1200;
    constexpr int image_height = std::max<int>(image_width / aspect_ratio, 1);

    // camera
    constexpr double vfov = 20.;

    constexpr point3 lookfrom = point3(13., 2., 3.);   // Point camera is looking from
    constexpr point3 lookat = point3(0., 0., 0.);  // Point camera is looking at
    constexpr vec3   vup = vec3(0., 1., 0.);     // Camera-relative "up" direction

    constexpr double defocus_angle = .6;  // Variation angle of rays through each pixel
    constexpr double focus_dist = 10.;    // Distance from camera lookfrom point to plane of perfect focus

    const vec3 w = glm::normalize(lookfrom - lookat);
    const vec3 u = glm::normalize(glm::cross(vup, w));
    const vec3 v = glm::cross(w, u);

    constexpr auto theta = geom::degrees_to_radians(vfov);
    const auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;

    const auto viewport_width = viewport_height * (double(image_width) / image_height);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    const auto pixel_delta_u = viewport_u * (1.0 / image_width);
    const auto pixel_delta_v = viewport_v * (1.0 / image_height);

    // Calculate the location of the upper left pixel.
    const auto viewport_upper_left = lookfrom - (focus_dist * w) - viewport_u / 2. - viewport_v / 2.;
    const auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    const auto defocus_radius = focus_dist * std::tan(geom::degrees_to_radians(defocus_angle / 2));
    const auto defocus_disk_u = u * defocus_radius;
    const auto defocus_disk_v = v * defocus_radius;

    render::Camera cam;
    cam.width = image_width;
    cam.height = image_height;
    cam.camera_center = lookfrom;
    cam.u = u;
    cam.v = v;
    cam.w = w;

    cam.pixel_00_loc = pixel00_loc;
    cam.pixel_delta_u = pixel_delta_u;
    cam.pixel_delta_v = pixel_delta_v;

    cam.defocus_angle = defocus_angle;
    cam.defocus_disk_u = defocus_disk_u;
    cam.defocus_disk_v = defocus_disk_v;
    return cam;
}


int main() {
    ECS ecs;

    ecs.registerComponent<geom::Sphere>();
    ecs.registerComponent<render::Material>();
    ecs.registerComponent<geom::Bounds>();

    auto& renderSystem = ecs.registerSystem<render::RenderSystem>();
    auto& boundsSystem = ecs.registerSystem<geom::BoundsUpdateSystem>();
    auto& bvhSystem = ecs.registerSystem<geom::BVHSystem>();
    EntityManager entityManager;




    const Entity thirdSphere = ecs.createEntity();
    ecs.addComponent(thirdSphere, geom::Sphere{ {4., 7., 0.}, {1.} });
    ecs.addComponent(thirdSphere, render::Material{ {0.7, 0.6, 0.5}, 1. , 0., 0. });
    ecs.addComponent(thirdSphere, geom::Bounds{});

    const Entity secondSphere = ecs.createEntity();
    ecs.addComponent(secondSphere, geom::Sphere{ {-4., 3., 0.}, {1.} });
    ecs.addComponent(secondSphere, render::Material{ {0.4, 0.2, 0.1}, 0., 0. });
    ecs.addComponent(secondSphere, geom::Bounds{});



    const Entity firstSphere = ecs.createEntity();
    ecs.addComponent(firstSphere, geom::Sphere{ {0., 1., 0.}, {1.} });
    ecs.addComponent(firstSphere, render::Material{ {0.1,0.2,0.5}, 0., 1. ,0.,1.5 });
    ecs.addComponent(firstSphere, geom::Bounds{});



    const Entity ground = ecs.createEntity();
    ecs.addComponent(ground, geom::Sphere{ {0., -1000., 0.}, 1. });
    ecs.addComponent(ground, render::Material{ {0.5, 0.5, 0.5}, 0., 0. });
    ecs.addComponent(ground, geom::Bounds{});


    


    // const Entity thirdSphere = ecs.createEntity();
    // ecs.addComponent(thirdSphere, geom::Sphere{ {4., 1., 0.}, {1.} });
    // ecs.addComponent(thirdSphere, render::Material{ {0.7, 0.6, 0.5}, 1. , 0., 0. });
    // ecs.addComponent(thirdSphere, geom::Bounds{});

    // const Entity secondSphere = ecs.createEntity();
    // ecs.addComponent(secondSphere, geom::Sphere{ {-4., 3., 0.}, {1.} });
    // ecs.addComponent(secondSphere, render::Material{ {0.4, 0.2, 0.1}, 0., 0. });
    // ecs.addComponent(secondSphere, geom::Bounds{});



    // const Entity firstSphere = ecs.createEntity();
    // ecs.addComponent(firstSphere, geom::Sphere{ {0., 1., 0.}, {1.} });
    // ecs.addComponent(firstSphere, render::Material{ {0.1,0.2,0.5}, 0., 1. ,0.,1.5 });
    // ecs.addComponent(firstSphere, geom::Bounds{});



    // const Entity ground = ecs.createEntity();
    // ecs.addComponent(ground, geom::Sphere{ {0., -1000., 0.}, 1000. });
    // ecs.addComponent(ground, render::Material{ {0.5, 0.5, 0.5}, 0., 0. });
    // ecs.addComponent(ground, geom::Bounds{});

    RNG rng = RNG(3);

    // for (int a = -11; a < 11; a++) {
    //     for (int b = -11; b < 11; b++) {
    //         auto choose_mat = rng.random_double();
    //         point3 center(a + 0.9 * rng.random_double(), 0.2, b + 0.9 * rng.random_double());

    //         const Entity sphere = ecs.createEntity();

    //         if ((center - point3(4, 0.2, 0)).length() > 0.9) {

    //             if (choose_mat < 0.8) {
    //                 // diffuse
    //                 const color albedo = geom::random_vec3(rng) * geom::random_vec3(rng);
    //                 const auto direction = vec3(0, rng.random_double(0, .5), 0);
    //                 ecs.addComponent(sphere, geom::Sphere{ center, 0.2 , direction });
    //                 ecs.addComponent(sphere, render::Material{ albedo, 0. , 0. });
    //                 ecs.addComponent(sphere, geom::Bounds{});
    //             }
    //             else if (choose_mat < 0.95) {
    //                 // metal
    //                 const color albedo = geom::random_vec3(rng);
    //                 const auto fuzz = rng.random_double(0, 0.5);
    //                 ecs.addComponent(sphere, geom::Sphere{ center, 0.2 });
    //                 ecs.addComponent(sphere, render::Material{ albedo, 1. , 0., fuzz });
    //                 ecs.addComponent(sphere, geom::Bounds{});
    //             }
    //             else {
    //                 // glass
    //                 ecs.addComponent(sphere, geom::Sphere{ center, 0.2 });
    //                 ecs.addComponent(sphere, render::Material{ {0.0, 0.0, 0.0}, 0., 1., 0.,1.5 });
    //                 ecs.addComponent(sphere, geom::Bounds{});
    //             }
    //         }
    //     }
    // }


    const int channels = 3; // RGB

    render::Camera cam = create_camera();

    // TODO: implement view cache invalidation, probably via registration in the ecs
    render::RenderView render_view{
            ecs.getComponentArray<geom::Sphere>(),
            ecs.getComponentArray<render::Material>()
    };

    geom::BoundsView bounds_view{
            ecs.getComponentArray<geom::Sphere>(),
            ecs.getComponentArray<geom::Bounds>()
    };

    geom::BVHView bvh_view{
            ecs.getComponentArray<geom::Bounds>()
    };

    boundsSystem.update_bounds(bounds_view);
    bvhSystem.build(bvh_view, rng);

    auto t1 = std::chrono::high_resolution_clock::now();
    const auto image = renderSystem.render_ecs(
        render_view,
        cam,
        rng
    );
    auto t2 = std::chrono::high_resolution_clock::now();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::clog << "render took " << sec.count() << "s " << (ms - sec).count() << "ms" << std::endl;
    std::clog << "Image data created successfully!" << std::endl;

    if (stbi_write_hdr("../../dummy.hdr", cam.width, cam.height, channels, image.data())) {
        std::clog << "Saved dummy.hdr successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to save dummy.hdr!" << std::endl;
        return 1;
    }

    return 0;

}