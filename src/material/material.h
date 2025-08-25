#ifndef MATERIAL_H
#define MATERIAL_H

#include "../common.h"

namespace render {
  struct Material {
    color albedo;
    double metallic;
  };

}

#endif