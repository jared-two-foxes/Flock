
#include "Flocking.hpp"

void Attraction( entity_t& entity, std::vector<entity_t >& entities )
{
	for( auto& e : entities ) 
	{
		if( entity.identifier == e.identifier ) 
    {
      continue;
    }

    // Move e towards entity.
    vector4_t dir  = entity.position - e.position;
    float     len  = Length( dir );
    vector4_t norm = Normalize( dir );
    e.position     = e.position + Truncate( norm * e.speed, len );
	}
}

void Seperation( entity_t& entity, std::vector<entity_t >& entities )
{
  for( auto& e : entities ) 
  {
    if( entity.identifier == e.identifier ) 
    {
      continue;
    }

    // Move e towards entity.
    vector4_t dir  = e.position - entity.position;
    vector4_t norm = Normalize( dir );
    e.position     = e.position + ( norm * e.speed );
  }
}

//void Cohesion( entity_t& entity, std::vector<entity_t >& entities )
//{}