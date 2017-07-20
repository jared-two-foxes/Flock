#ifndef _FLOCK_CLIENT_PLAYERCONTROLLER_H__
#define _FLOCK_CLIENT_PLAYERCONTROLLER_H__

#include <Client/ActionBinding.hpp>

#include <Common/Math/Math.hpp>

struct entity_t;

class PlayerController
///
/// Encapsulates all the logic needed for one player to interact with a dedicated "player" entities
/// in the game world.
///
{
private:
  const int m_identifier;
  std::vector<KeyBinding > m_inputBindings;
  std::vector<PlayerAction > m_activeActions;
  std::string  m_lastCommand;     ///< The last command that was sent to the server for this player.

  public:
    explicit PlayerController( const int identifier );

    const int Identifier() const { return m_identifier; }
    const std::vector<PlayerAction >& ActiveActions() const { return m_activeActions; }

    void KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys );
    void KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys );
    
    void SetBinding( PlayerAction action, Nebulae::KeyCode key );

};

#endif // _FLOCK_CLIENT_PLAYERCONTROLLER_H__