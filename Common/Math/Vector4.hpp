#ifndef MATH_VECTOR4_H__
#define MATH_VECTOR4_H__

struct vector4_t
{
public:
	float x, y, z, w;

  public:
    vector4_t() {}
    vector4_t( float x, float y, float z, float w );

    vector4_t operator+ ( const vector4_t& lhs );
    vector4_t operator- ( const vector4_t& lhs );
    vector4_t operator* ( const float s );
    vector4_t operator/ ( const float s );

};

float Length( const vector4_t& v );
float LengthSq( const vector4_t& v );
vector4_t Normalize( const vector4_t& v );
vector4_t Truncate( const vector4_t& v, float max );
float Dot3( const vector4_t& a, const vector4_t& b );
float Dot4( const vector4_t& a, const vector4_t& b );
float Cross3( const vector4_t& a, const vector4_t& b );
float Cross4( const vector4_t& a, const vector4_t& b );

#endif // MATH_VECTOR4_H__
