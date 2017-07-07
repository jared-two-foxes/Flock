

inline plane_t::plane_t( const plane_t& rhs ) : 
  n( rhs.n )
{}

inline plane_t::plane_t( float a, float b, float c, float d ) : 
  n( a, b, c, d )
{}

inline plane_t::plane_t( const vector4_t& normal, const float constant ) : 
  n( normal.x, normal.y, normal.z, constant )
{}

inline plane_t::plane_t( const vector4_t& normal, const vector4_t& point )
{
  n = normal;
  n.w = Length( point );
}


//
// Accessors
//

inline vector4_t plane_t::getNormal() const
{
  return vector4_t( n.x, n.y, n.z, 0 );
}


//
// Free Functions
//

inline float Distance( const plane_t& plane, const vector4_t& pt )
{
	return Dot3( plane.n, pt ) + plane.n.w;
}