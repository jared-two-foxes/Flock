#ifndef _FLOCK_CLIENT_ACTIONBINDING_H__
#define _FLOCK_CLIENT_ACTIONBINDING_H__

#include <Nebulae/Common/Common.h>

enum class PlayerAction
{ 
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

struct KeyBinding
{
  Nebulae::KeyCode key;
  PlayerAction     action;
};


#endif // _FLOCK_CLIENT_ACTIONBINDING_H__