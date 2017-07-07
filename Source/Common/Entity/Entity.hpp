#ifndef ENTITY_H__
#define ENTITY_H__

#include <Common/Math/Math.hpp>

struct entity_t
{
	int       identifier;
  float     speed;
  float     radius;
	vector2_t position;
  vector2_t direction;
  bool      player;
};

#endif // ENTITY_H__