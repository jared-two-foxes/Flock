

inline plane_t::plane_t( const plane_t& rhs )
  : p( rhs.p )
{}

inline plane_t::plane_t( float a, float b, float c, float d )
  : p( a, b, c, d )
{}

inline plane_t::plane_t( const vector4_t& normal, const float constant )
  : p( normal.x, normal.y, normal.z, constant )
{}

inline plane_t::plane_t( const vector4_t& normal, const vector4_t& point )
{
  p = normal;
  p.w = Length( point );
}


//
// Accessors
//

inline vector4_t plane_t::getNormal() const
{
  return vector4_t( p.x, p.y, p.z, 0 );
}


//
// Free Functions
//

inline float Distance( const plane_t& plane, const vector4_t& pt )
{
	return Dot3( plane.p, pt ) + plane.p.w;
}