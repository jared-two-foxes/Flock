#ifndef __FLOCK_INPUTLISTENER_H__
#define __FLOCK_INPUTLISTENER_H__

#include <Nebulae/Alpha/Alpha.h>

class ClientState;

class ClientInputListener : public Nebulae::InputListener
{
public:
  ClientState* state;

  virtual void KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys ) override;
  virtual void KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys ) override;

}; //ClientInputListener

#endif