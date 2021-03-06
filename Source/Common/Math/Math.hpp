#ifndef MATH_H__
#define MATH_H__

#include <utility>    //for std::pair
#include <limits>     //for numeric_limits in Ray.inl

#define EPS 0.001f

// Forward Declare all the structures
struct box_t;
struct plane_t;
struct matrix2_t;
struct ray_t;
struct rect_t;
struct vector2_t;
struct vector4_t;

// Include all the definitions
#include <Common/Math/Vector2.hpp>
#include <Common/Math/Vector4.hpp>
#include <Common/Math/Matrix.hpp>
#include <Common/Math/Box.hpp>
#include <Common/Math/Plane.hpp>
#include <Common/Math/Ray.hpp>
#include <Common/Math/Rect.hpp>

// Include basic math routinues
#include <Common/Math/MathUtils.hpp>

// All the math functions are implemented using inl files, included here.
#include <Common/Math/Vector2.inl>
#include <Common/Math/Vector4.inl>
#include <Common/Math/Matrix.inl>
#include <Common/Math/Box.inl>
#include <Common/Math/Plane.inl>
#include <Common/Math/Ray.inl>
#include <Common/Math/Rect.inl>

#endif // MATH_H__