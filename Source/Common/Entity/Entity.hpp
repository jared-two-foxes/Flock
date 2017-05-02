#ifndef ENTITY_H__
#define ENTITY_H__

#include <Common/Math/Math.hpp>

struct entity_t
{
	int       identifier;
  float     speed;
	vector4_t position;
	//vector4_t rotation; //< quaternion	
};

#endif // ENTITY_H__