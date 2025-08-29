#ifndef CAMERA_H
#define CAMERA_H

#include "geometry/ray.h"

namespace render {


    struct Camera {

        int width, height;
        int samples_per_pixel = 100; // Count of random samples for each pixel
        int max_depth = 100; // Maximum number of ray bounces into scene
        vec3 camera_center; // Center of the camera
        vec3 pixel_00_loc; // Location of the upper left pixel
        vec3 pixel_delta_u; // Delta vector across the horizontal viewport edge
        vec3 pixel_delta_v; // Delta vector down the vertical viewport edge
        Ray camera_ray(int i, int j) const {
            auto pixel_center = pixel_00_loc + (double(i) * pixel_delta_u) + (double(j) * pixel_delta_v);
            auto ray_direction = glm::normalize(pixel_center - camera_center);
            return Ray(camera_center, ray_direction, color(1., 1., 1.), i * width + j, max_depth);
        } // Ray from the camera center through the pixel center
        Ray get_ray(int i, int j) const {
            double offset_x = random_double(-0.5, 0.5);
            double offset_y = random_double(-0.5, 0.5);
            auto pixel_center = pixel_00_loc + ((double(i) + offset_x) * pixel_delta_u) + ((double(j) + offset_y) * pixel_delta_v);
            auto ray_direction = glm::normalize(pixel_center - camera_center);
            return Ray(camera_center, ray_direction, color(1., 1., 1.), j * width + i, max_depth);
        } // Ray from the camera center through the pixel center

    };


}
#endif // CAMERA_H