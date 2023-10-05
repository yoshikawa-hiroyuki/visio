#ifndef GEARS_GEOMETRY_VECTOR3_H
#define GEARS_GEOMETRY_VECTOR3_H

#define _USE_MATH_DEFINES
#include <cmath>

namespace gears {
namespace geometry {

#define ELEM0_NAME x
#define ELEM1_NAME y
#define ELEM2_NAME z
#define TEMPLATE_NAME vector3
#include <gears/_3elems.h>
#undef TEMPLATE_NAME
#undef ELEM2_NAME
#undef ELEM1_NAME
#undef ELEM0_NAME

} // end of namespace geometry
} // end of namespace gears

#endif // GEARS_GEOMETRY_VECTOR3_H
