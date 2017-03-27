#ifndef MATH_BOX_H__
#define MATH_BOX_H__

struct box_t
//
// Simple Axis Aligned Box.
//
{
public:
  vector4_t min;
  vector4_t max;

  public:
    box_t() {}
    box_t( const vector4_t& min, const vector4_t& max );

};

bool ContainsPoint( const box_t& box, const vector4_t& p );
float Intersect( const box_t& box, const vector4_t origin, const vector4_t& direction );

#endif // MATH_BOX_H__