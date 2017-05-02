
inline box_t::box_t( const vector4_t& a, const vector4_t& b ) 
  : min( a ), max( b )
{}

//bool ContainsPoint( const box_t& box, const vector4_t& p );
//
//inline float Intersect( const box_t& box, const vector4_t orig, const vector4_t& dir )
//{
//	float tmin = ( box.min.x - orig.x ) / dir.x; 
//  float tmax = ( box.max.x - orig.x ) / dir.x; 
//
//  if ( tmin > tmax ) 
//  {
//    swap( tmin, tmax ); 
//  }
//
//  float tymin = ( box.min.y - orig.y ) / dir.y; 
//  float tymax = ( box.max.y - orig.y ) / dir.y; 
//
//  if ( tymin > tymax ) 
//  { 
//    swap( tymin, tymax ); 
//  }
//
//  if ( (tmin > tymax) || (tymin > tmax) ) 
//  {
//    return false; 
//  }
//
//  if ( tymin > tmin ) 
//  {
//    tmin = tymin; 
//  }
//
//  if ( tymax < tmax ) 
//  {
//    tmax = tymax; 
//  }
//
//  float tzmin = ( box.min.z - orig.z ) / dir.z; 
//  float tzmax = ( box.max.z - orig.z ) / dir.z; 
//
//  if ( tzmin > tzmax ) 
//  {
//    swap( tzmin, tzmax ); 
//  }
//
//  if ( (tmin > tzmax) || (tzmin > tmax) ) 
//  { 
//    return false; 
//  }
//
//  if ( tzmin > tmin ) 
//  {
//    tmin = tzmin; 
//  }
//
//  if ( tzmax < tmax ) 
//  {
//    tmax = tzmax; 
//  }
//
//  return true; 
//}