#ifndef GEARS_GEOMETRY_MATRIX33_H
#define GEARS_GEOMETRY_MATRIX33_H

#include <gears/geometry/vector3.h>

namespace gears {
namespace geometry {

#define VECTOR0_NAME vx
#define VECTOR1_NAME vy
#define VECTOR2_NAME vz
#define VECTOR_TEMPLATE_NAME vector3
#define TEMPLATE_NAME matrix33
#include <gears/_3x3elems.h>
#undef TEMPLATE_NAME
#undef VECTOR_TEMPLATE_NAME
#undef VECTOR2_NAME
#undef VECTOR1_NAME
#undef VECTOR0_NAME

} // namespace geometry
} // end of namespace gears

#endif // GEARS_GEOMETRY_MATRIX33_H
