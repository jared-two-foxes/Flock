#ifndef _FLOCK_CLIENT_DRAWINGHELPER_HPP__
#define _FLOCK_CLIENT_DRAWINGHELPER_HPP__

#include <Common/Math/Math.hpp>

//#include <cstdlib>
#include <vector>


class DrawingHelper
///
/// A class to assist in drawing the simple shapes representing the in game entities to the screen.
///
{
public:
  static std::size_t DrawCircle( const vector2_t& p, float r, int num_segments, const vector4_t& d, std::vector<float >* vertices );
  static std::size_t DrawArrow( const vector2_t& p, const vector2_t& dir, float r, const vector4_t& d, std::vector<float >* vertices );

};

#endif // _FLOCK_CLIENT_DRAWINGHELPER_HPP__