#ifndef GEARS_GEOMETRY_MATRIX22_H
#define GEARS_GEOMETRY_MATRIX22_H

#include <gears/geometry/vector2.h>

namespace gears {
namespace geometry {

#define VECTOR0_NAME vx
#define VECTOR1_NAME vy
#define VECTOR_TEMPLATE_NAME vector2
#define TEMPLATE_NAME matrix22
#include <gears/_2x2elems.h>
#undef TEMPLATE_NAME
#undef VECTOR_TEMPLATE_NAME
#undef VECTOR1_NAME
#undef VECTOR0_NAME

} // namespace geometry
} // end of namespace gears

#endif // GEARS_GEOMETRY_MATRIX22_H
