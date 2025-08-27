#ifndef MATERIAL_H
#define MATERIAL_H

#include "../common.h"

namespace render {
  struct Material {
    color albedo;
    double metallic;
    double dielectric;
    double fuzz = 0.;
    double refraction_index = 1.;
  };

}

#endif