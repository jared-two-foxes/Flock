
#include "DrawingHelper.hpp"


std::size_t
DrawingHelper::DrawCircle( const vector2_t& p, float r, int num_segments, const vector4_t& d, std::vector<float >* vertices )
// GL_LINE_LOOP
{
  float theta = 2 * 3.1415926f / float( num_segments );
  float c = cosf( theta );//precalculate the sine and cosine
  float s = sinf( theta );
  float t;

  float x = r; //we start at angle = 0 
  float y = 0;

  std::size_t i = 0, ii = 0;

  // To create using a triangle fan.
  ( *vertices )[ i++ ] = p.x;
  ( *vertices )[ i++ ] = p.y;
  ( *vertices )[ i++ ] = d.x;
  ( *vertices )[ i++ ] = d.y;
  ( *vertices )[ i++ ] = d.z;
  ( *vertices )[ i++ ] = d.w;

  for ( ; ii < num_segments + 1; ii++ )
  {
    // Output vertex 
    ( *vertices )[ i++ ] = x + p.x;
    ( *vertices )[ i++ ] = y + p.y;

    // Output colour
    ( *vertices )[ i++ ] = d.x;
    ( *vertices )[ i++ ] = d.y;
    ( *vertices )[ i++ ] = d.z;
    ( *vertices )[ i++ ] = d.w;

    //apply the rotation matrix
    t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }

  return ( ii + 1 );
}


std::size_t
DrawingHelper::DrawArrow( const vector2_t& p, const vector2_t& dir, float r, const vector4_t& d, std::vector<float >* vertices )
// GL_TRIANGLES
{
  vector2_t offsets[] = { {0,r}, {0,-r}, {r,-r}, {0,r}, {-r,-r}, {0,-r} };

  static vector2_t up( 0.0f, 1.0f ), left( -1.0f, 0 );
  float perp_dot = Dot( left, dir );
  float dot = Dot( up, dir );
  float a = atan2( perp_dot, dot );
  float c = cosf( a );
  float s = sinf( a );
  matrix2_t rot ( c, -s, s, c );
  
  std::size_t i = 0, ii = 0;
  for ( ; ii < 6; ++ii )
  {
    vector2_t off = rot * offsets[ ii ];

    ( *vertices )[ i++ ] = p.x + off.x;
    ( *vertices )[ i++ ] = p.y + off.y;
    ( *vertices )[ i++ ] = d.x;
    ( *vertices )[ i++ ] = d.y;
    ( *vertices )[ i++ ] = d.z;
    ( *vertices )[ i++ ] = d.w;
  }

  return ii;
}