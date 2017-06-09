
#include "Flocking.hpp"
#include <assert.h>


void Attraction( entity_t& entity, entity_t& target )
{
  assert( entity.identifier != target.identifier );

  // Move e towards entity.
  vector2_t dir = target.position - entity.position;
  float     len = Length( dir );
  if ( len > 0 )
  {
    vector2_t norm = Normalize( dir );
    entity.position = entity.position + Truncate( norm * entity.speed, len );
  }
}


void Attraction( entity_t& entity, std::vector<entity_t >& entities )
{
  for ( auto& e : entities )
  {
    if ( entity.identifier == e.identifier )
    {
      continue;
    }

    // Move e towards entity.
    vector2_t dir = entity.position - e.position;
    float     len = Length( dir );
    if ( len > 0 )
    {
      vector2_t norm = Normalize( dir );
      entity.position = entity.position + Truncate( norm * entity.speed, len );
    }
  }
}


void Seperation( entity_t& entity, std::vector<entity_t >& entities )
{
  for ( auto& e : entities )
  {
    if ( entity.identifier == e.identifier )
    {
      continue;
    }

    // Move e towards entity, unless they occupy the same spot in which case do nothing.
    vector2_t dir = e.position - entity.position;
    if ( Length( dir ) > 0 )
    {
      vector2_t norm = Normalize( dir );
      e.position = e.position + ( norm * e.speed );
    }
  }
}

//void Cohesion( entity_t& entity, std::vector<entity_t >& entities )
//{}