#ifndef MATH_PLANE_H__
#define MATH_PLANE_H__

struct plane_t
{
public:
  vector4_t p; // (x,yz) represent a normal, & w-component represents the distance along normal to origin.

  public:
    plane_t() {}
    plane_t( const plane_t& rhs );
    plane_t( float a, float b, float c, float d );
    plane_t( const vector4_t& normal, const float constant );
    plane_t( const vector4_t& normal, const vector4_t& point );
    plane_t( const vector4_t& point1, const vector4_t& point2, const vector4_t& point3 );

    vector4_t getNormal() const;
};

float Distance( const plane_t& plane, const vector4_t& pt );

#endif // MATH_PLANE_H__