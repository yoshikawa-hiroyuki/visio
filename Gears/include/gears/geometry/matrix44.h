#ifndef GEARS_GEOMETRY_MATRIX44_H
#define GEARS_GEOMETRY_MATRIX44_H

#include <gears/geometry/vector4.h>
#include <gears/geometry/matrix33.h>

namespace gears {
namespace geometry {

#define VECTOR0_NAME vx
#define VECTOR1_NAME vy
#define VECTOR2_NAME vz
#define VECTOR3_NAME vw
#define VECTOR_TEMPLATE_NAME vector4
#define TEMPLATE_NAME matrix44
#define MATRIX33_TEMPLATE_NAME matrix33
#include <gears/_4x4elems.h>
#undef MATRIX33_TEMPLATE_NAME
#undef TEMPLATE_NAME
#undef VECTOR_TEMPLATE_NAME
#undef VECTOR3_NAME
#undef VECTOR2_NAME
#undef VECTOR1_NAME
#undef VECTOR0_NAME

} // namespace geometry
} // end of namespace gears

#endif // GEARS_GEOMETRY_MATRIX44_H
