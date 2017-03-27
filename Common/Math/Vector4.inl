
inline vector4_t::vector4_t( float a, float b, float c, float d ) 
  : x( a ), y( b ), z( c ), w( d )
{}

inline vector4_t vector4_t::operator + ( const vector4_t& rhs )
{
	return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
}

inline vector4_t vector4_t::operator - ( const vector4_t& rhs )
{
	return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
}

inline vector4_t vector4_t::operator* ( const float s ) 
{
  return { x * s, y * s, z * s, w * s };
}

inline vector4_t vector4_t::operator/ ( const float s )
{
  float inv = 1.0f / s;
  return { x * inv, y * inv, z * inv, w * inv };
}



// ---------------
// Free Functions
// ----------------

inline float Length( const vector4_t& v ) 
{
  return sqrtf( v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w ); 
}

inline float LengthSq( const vector4_t& v ) 
{
  return ( v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w ); 
}

inline vector4_t Normalize( const vector4_t& v ) 
{
  float invLen = 1.0f / Length( v );
  return { v.x * invLen, v.y * invLen, v.z * invLen, v.w * invLen };
}

inline vector4_t Truncate( const vector4_t& v, float max ) 
{
  float s = max * max / LengthSq( v );
  float inv = 1.0f / s;
  return { v.x * inv, v.y * inv, v.z * inv, v.w * inv };
}

inline float Dot3( const vector4_t& a, const vector4_t& b ) 
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float Dot4( const vector4_t& a, const vector4_t& b )
{
  return a.x * b.x + a.y * b.y + a.z * b.z + a. w * b.w;
}
