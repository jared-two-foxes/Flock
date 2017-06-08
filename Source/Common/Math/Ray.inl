
inline std::pair<bool, float > Intersects( const ray_t& ray, const plane_t& plane )
{
  float denom = Dot3( plane.n, ray.direction ); 
  if ( fabs( denom ) < std::numeric_limits<float >::epsilon() )
  {
    return std::pair<bool, float >( false, 0 );
  }
  else
  {
    float nom = Dot3( plane.n, ray.origin ) + plane.n.w;
    float t = -( nom / denom );
    return std::pair<bool, float >( t >= 0, t );
  }
}

//std::pair<bool, float > Intersects( const ray_t& ray, const std::vector<plane_t >& planes )