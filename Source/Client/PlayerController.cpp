
#include "PlayerController.hpp"

#include <Common/Entity/Entity.hpp>


PlayerController::PlayerController( const int identifier ) :
  m_identifier( identifier )
{}


void
PlayerController::KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  // Iterate the keybindings to see if this keypress should do anything.
  auto it = std::find_if( m_inputBindings.begin(), m_inputBindings.end(), 
    [ & ]( KeyBinding& binding ) {
      return ( binding.key == keyCode );
    } );

  if ( it != m_inputBindings.end() )
  {
    // Only add the action into the active actions list so long as its not already pressed.
    auto j = std::find( m_activeActions.begin(), m_activeActions.end(), it->action );
    if ( j == m_activeActions.end() )
    {
      m_activeActions.push_back( it->action );
    }
  }
}


void
PlayerController::KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  // Iterate the keybindings to see if this keypress maps to anything.
  auto it = std::find_if( m_inputBindings.begin(), m_inputBindings.end(), 
    [ & ]( KeyBinding& binding ) {
      return ( binding.key == keyCode );
    } );

  // If found remove from the action queue.
  if ( it != m_inputBindings.end() )
  {
    m_activeActions.erase( std::remove( m_activeActions.begin(), m_activeActions.end(), it->action ) );
  }
}


void 
PlayerController::SetBinding( PlayerAction action_, Nebulae::KeyCode key_ )
{
  m_inputBindings.push_back( KeyBinding() );
  KeyBinding& binding = m_inputBindings.back();
  binding.action = action_;
  binding.key = key_;
}