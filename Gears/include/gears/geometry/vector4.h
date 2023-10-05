#ifndef GEARS_GEOMETRY_VECTOR4_H
#define GEARS_GEOMETRY_VECTOR4_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <gears/geometry/vector3.h>

namespace gears {
namespace geometry {

#define ELEM0_NAME x
#define ELEM1_NAME y
#define ELEM2_NAME z
#define ELEM3_NAME w
#define TEMPLATE_NAME vector4
#define VECTOR3_TEMPLATE_NAME vector3
#include <gears/_4elems.h>
#undef VECTOR3_TEMPLATE_NAME
#undef TEMPLATE_NAME
#undef ELEM3_NAME
#undef ELEM2_NAME
#undef ELEM1_NAME
#undef ELEM0_NAME

} // end of namespace geometry
} // end of namespace gears

#endif // GEARS_GEOMETRY_VECTOR4_H
