#ifndef MATH_UTILS_H__
#define MATH_UTILS_H__

#include <cstdlib>

inline  float RandFloat()            
{
  return static_cast<float >( rand() ) / static_cast<float >( RAND_MAX );
}

inline float RandFloat( float lo, float hi )    
{
  return lo + static_cast<float >( rand() ) / ( static_cast<float >( RAND_MAX ) / ( hi - lo ) );
}

#endif // MATH_UTILS_H__