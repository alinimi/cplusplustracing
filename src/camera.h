#ifndef CAMERA_H
#define CAMERA_H

#include "geometry/ray.h"

namespace render {


    struct Camera {

        int width, height;
        int samples_per_pixel = 100; // Count of random samples for each pixel
        int max_depth = 100; // Maximum number of ray bounces into scene
        vec3 camera_center; // Center of the camera
        vec3   u, v, w;              // Camera frame basis vectors
        vec3 pixel_00_loc; // Location of the upper left pixel
        vec3 pixel_delta_u; // Delta vector across the horizontal viewport edge
        vec3 pixel_delta_v; // Delta vector down the vertical viewport edge
        double defocus_angle;
        vec3 defocus_disk_u, defocus_disk_v;

        point3 defocus_disk_sample() const {
            // Returns a random point in the camera defocus disk.
            auto p = random_in_unit_disk();
            return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        Ray get_ray(int i, int j) const {
            assert(i >= 0 && i < width && j >= 0 && j < height);
            const double offset_x = random_double(-0.5, 0.5);
            const double offset_y = random_double(-0.5, 0.5);
            const auto pixel_sample = pixel_00_loc + ((double(i) + offset_x) * pixel_delta_u) + ((double(j) + offset_y) * pixel_delta_v);
            const auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
            const auto ray_direction = glm::normalize(pixel_sample - ray_origin);
            return Ray(ray_origin, ray_direction, color(1., 1., 1.), j * width + i, max_depth);
        } // Ray from the camera center through the pixel center


    };


}
#endif // CAMERA_H