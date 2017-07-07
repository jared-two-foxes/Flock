#ifndef MATH_VECTOR2_H__
#define MATH_VECTOR2_H__

struct vector2_t
{
public:
	float x, y;

  public:
    vector2_t() {}
    vector2_t( float x, float y );

    vector2_t operator+ ( const vector2_t& lhs );
    vector2_t operator- ( const vector2_t& lhs );
    vector2_t operator* ( const float s );
    vector2_t operator/ ( const float s );

};

bool operator ==( const vector2_t& a, const vector2_t& b );

float Length( const vector2_t& v );
float LengthSq( const vector2_t& v );
vector2_t Normalize( const vector2_t& v );
vector2_t Truncate( const vector2_t& v, float max );
float Dot( const vector2_t& a, const vector2_t& b );
float Cross( const vector2_t& a, const vector2_t& b );

#endif // MATH_VECTOR2_H__
