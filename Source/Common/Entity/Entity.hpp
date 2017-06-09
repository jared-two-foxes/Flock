#ifndef ENTITY_H__
#define ENTITY_H__

#include <Common/Math/Math.hpp>

struct entity_t
{
	int       identifier;
  float     speed;
	vector2_t position;
  vector2_t direction;
  vector4_t colour;
  int       player;
};

#endif // ENTITY_H__