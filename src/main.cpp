#include <iostream>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <glm/glm.hpp>
#include "ecs/ECS.h"
#include "geometry/ray.h"
#include "geometry/hittable.h"
#include "render_system.h"

render::Camera create_camera() {
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;


    // Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);
    auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u * (1.0 / image_width);
    auto pixel_delta_v = viewport_v * (1.0 / image_height);

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center
        - vec3(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


    render::Camera cam;
    cam.width = image_width;
    cam.height = image_height;
    cam.camera_center = camera_center;
    cam.pixel_00_loc = pixel00_loc;
    cam.pixel_delta_u = pixel_delta_u;
    cam.pixel_delta_v = pixel_delta_v;
    return cam;
}


int main() {
    ECS ecs;




    ecs.registerComponent<render::Sphere>();
    std::shared_ptr<render::RenderSystem> renderSystem = ecs.registerSystem<render::RenderSystem>();

    Signature renderSignature;
    renderSignature.set(ecs.getComponentType<render::Sphere>());
    ecs.setSystemSignature<render::RenderSystem>(renderSignature);


    // Signature signature;
    // signature.set(gCoordinator.GetComponentType<Gravity>());
    // signature.set(gCoordinator.GetComponentType<RigidBody>());
    // signature.set(gCoordinator.GetComponentType<Transform>());
    // gCoordinator.SetSystemSignature<PhysicsSystem>(signature);

    // std::vector<Entity> entities(MAX_ENTITIES);


    EntityManager entityManager;

    Entity firstSphere = ecs.createEntity();
    ecs.addComponent(firstSphere, render::Sphere{ {0.,0.,-1.},{0.5} });
    // Entity ground = ecs.createEntity();
    // ecs.addComponent(ground, render::Sphere{ {0.,-100.5,-1.},{100.} });

    const int channels = 3; // RGB

    render::Camera cam = create_camera();
    auto image = renderSystem->render(ecs, cam);
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