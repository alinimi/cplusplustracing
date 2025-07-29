#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

namespace render {


    struct Camera {
        int width, height;
        vec3 camera_center; // Center of the camera
        vec3 pixel_00_loc; // Location of the upper left pixel
        vec3 pixel_delta_u; // Delta vector across the horizontal viewport edge
        vec3 pixel_delta_v; // Delta vector down the vertical viewport edge
        Ray camera_ray(int i, int j) const {
            auto pixel_center = pixel_00_loc + (double(i) * pixel_delta_u) + (double(j) * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            return Ray(camera_center, ray_direction);
        } // Ray from the camera center through the pixel center
    };


}
#endif // CAMERA_H