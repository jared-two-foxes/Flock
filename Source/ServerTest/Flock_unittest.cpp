
#include "Common/Entity/Flocking.hpp"
#include "gtest/gtest.h"



TEST( Flock, Attraction_EntityDirectionUpdated )
{
  entity_t e, t;
  e.position = vector2_t( 0, 0 );
  e.speed = 1.0f;
  t.position = vector2_t( 10, 10 );

  vector2_t facing = Normalize( t.position - e.position );
  Attraction( e, t );

  EXPECT_EQ( e.direction, facing );
}
