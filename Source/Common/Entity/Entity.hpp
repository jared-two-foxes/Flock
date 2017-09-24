#ifndef ENTITY_H__
#define ENTITY_H__

#include <Common/Math/Math.hpp>

enum EntityState
{
  ALIVE,
  DEAD
};

struct entity_t
{
	int       identifier;
  float     speed;
  float     radius;
	vector2_t position;
  vector2_t direction;
  int       state;
  bool      player;
};

#endif // ENTITY_H__