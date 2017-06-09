#ifndef MATH_RECT_H__
#define MATH_RECT_H__

struct rect_t
//
// Simple Axis Aligned Rect.
//
{
public:
  vector2_t a; //min;
  vector2_t b; //max;

  public:
    rect_t() {}
    rect_t( const vector2_t& min, const vector2_t& max );

};

bool ContainsPoint( const rect_t& box, const vector2_t& p );
float Intersect( const rect_t& box, const vector2_t origin, const vector2_t& direction );

#endif // MATH_RECT_H__