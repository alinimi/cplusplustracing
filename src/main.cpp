#include <chrono>
#include <iostream>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <glm/glm.hpp>
#include "ecs/ECS.h"
#include "geometry/ray.h"
#include "geometry/hittable.h"
#include "material/material.h"
#include "render_system.h"

render::Camera create_camera() {
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    constexpr int image_height = std::max<int>(image_width / aspect_ratio, 1);

    // image_height = (image_height < 1) ? 1 : image_height;


    // Camera

    constexpr double vfov = 20.;

    constexpr point3 lookfrom = point3(-2, 2, 1);   // Point camera is looking from
    constexpr point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    constexpr vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction

    constexpr double defocus_angle = 10.0;  // Variation angle of rays through each pixel
    constexpr double focus_dist = 3.4;    // Distance from camera lookfrom point to plane of perfect focus

    const vec3 w = glm::normalize(lookfrom - lookat);
    const vec3 u = glm::normalize(glm::cross(vup, w));
    const vec3 v = glm::cross(w, u);

    constexpr auto theta = degrees_to_radians(vfov);
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

    const auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
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




    ecs.registerComponent<render::Sphere>();
    ecs.registerComponent<render::Material>();

    std::shared_ptr<render::RenderSystem> renderSystem = ecs.registerSystem<render::RenderSystem>();

    Signature renderSignature;
    renderSignature.set(ecs.getComponentType<render::Sphere>());
    renderSignature.set(ecs.getComponentType<render::Material>());

    ecs.setSystemSignature<render::RenderSystem>(renderSignature);


    // Signature signature;
    // signature.set(gCoordinator.GetComponentType<Gravity>());
    // signature.set(gCoordinator.GetComponentType<RigidBody>());
    // signature.set(gCoordinator.GetComponentType<Transform>());
    // gCoordinator.SetSystemSignature<PhysicsSystem>(signature);

    // std::vector<Entity> entities(MAX_ENTITIES);


    EntityManager entityManager;

    const Entity firstSphere = ecs.createEntity();
    ecs.addComponent(firstSphere, render::Sphere{ {0.,0.,-1.2}, {0.5} });
    ecs.addComponent(firstSphere, render::Material{ {0.1,0.2,0.5}, 0., 0. });

    const Entity ground = ecs.createEntity();
    ecs.addComponent(ground, render::Sphere{ {0.,-100.5,-1.}, {100.} });
    ecs.addComponent(ground, render::Material{ {0.8,0.8,0.}, 0., 0. });

    const Entity leftSphere = ecs.createEntity();
    ecs.addComponent(leftSphere, render::Sphere{ {-1.,0.,-1.}, {0.5} });
    ecs.addComponent(leftSphere, render::Material{ {0.2, 0.2, 0.2}, 0., 1., 0.,1.5 });

    const Entity insideSphere = ecs.createEntity();
    ecs.addComponent(insideSphere, render::Sphere{ {-1.,0.,-1.}, {0.4} });
    ecs.addComponent(insideSphere, render::Material{ {0.8, 0.8, 0.8}, 0., 1., 0., 1. / 1.5 });

    const Entity rightSphere = ecs.createEntity();
    ecs.addComponent(rightSphere, render::Sphere{ {1.,0.,-1.}, {0.5} });
    ecs.addComponent(rightSphere, render::Material{ {0.8, 0.6, 0.2}, 1. , 0., 1. });


    const int channels = 3; // RGB

    render::Camera cam = create_camera();

    auto t1 = std::chrono::high_resolution_clock::now();
    const auto image = renderSystem->render_ecs(ecs, cam);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::clog << "render took " << sec.count() << "s " << (ms - sec).count() << "ms" << std::endl;
    std::clog << "Image data created successfully!" << std::endl;


    // Write to HDR file using stb_image_write
    if (stbi_write_hdr("../../dummy.hdr", cam.width, cam.height, channels, image.data())) {
        std::clog << "Saved dummy.hdr successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to save dummy.hdr!" << std::endl;
        return 1;
    }

    return 0;

}