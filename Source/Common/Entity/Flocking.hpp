#ifndef FLOCKING_H__
#define FLOCKING_H__

#include <Common/Entity/Entity.hpp>

#include <vector>

void Attraction( entity_t& entity, entity_t& target );

void Attraction( std::vector<entity_t >& entities, entity_t& target );
void Seperation( entity_t& entity, std::vector<entity_t >& entities );
void Cohesion( entity_t& entity, std::vector<entity_t >& entities );

#endif // FLOCKING_H__