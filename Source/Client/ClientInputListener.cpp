
#include "ClientInputListener.hpp"

#include "ClientState.hpp"



void
ClientInputListener::KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  state->OnKeyUp( keyCode );
}