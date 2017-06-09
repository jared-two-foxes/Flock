
inline vector2_t::vector2_t( float a, float b ) 
  : x( a ), y( b )
{}

inline vector2_t vector2_t::operator + ( const vector2_t& rhs )
{
	return { x + rhs.x, y + rhs.y };
}

inline vector2_t vector2_t::operator - ( const vector2_t& rhs )
{
	return { x - rhs.x, y - rhs.y };
}

inline vector2_t vector2_t::operator* ( const float s ) 
{
  return { x * s, y * s };
}

inline vector2_t vector2_t::operator/ ( const float s )
{
  float inv = 1.0f / s;
  return { x * inv, y * inv };
}



// ---------------
// Free Functions
// ----------------

inline float Length( const vector2_t& v ) 
{
  return sqrtf( v.x * v.x + v.y * v.y ); 
}

inline float LengthSq( const vector2_t& v ) 
{
  return ( v.x * v.x + v.y * v.y ); 
}

inline vector2_t Normalize( const vector2_t& v ) 
{
  float invLen = 1.0f / Length( v );
  return { v.x * invLen, v.y * invLen };
}

inline vector2_t Truncate( const vector2_t& v, float max ) 
{
  float s = max * max / LengthSq( v );
  float inv = 1.0f / s;
  return { v.x * inv, v.y * inv };
}

inline float Dot( const vector2_t& a, const vector2_t& b ) 
{
  return a.x * b.x + a.y * b.y;
}