#ifndef MATH_RAY_H__
#define MATH_RAY_H__

struct ray_t
{
public:
  vector4_t origin;
  vector4_t direction;

};

std::pair<bool, float > Intersects( const ray_t& ray, const plane_t& plane );
//std::pair<bool, float > Intersects( const ray_t& ray, const std::vector<plane_t >& planes );

#endif // MATH_RAY_H__