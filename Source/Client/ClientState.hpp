#ifndef __FLOCK_CLIENTSTATE_H__
#define __FLOCK_CLIENTSTATE_H__


#include "Common/Entity/Entity.hpp"

#include <Nebulae/Beta/StateStack/State.h>
#include <Nebulae/Common/Common.h>

#include <zmq.hpp>

#include <thread>

namespace Nebulae {
  class Camera;
  class RenderSystem;
}


/** ClientState.
 */
class ClientState : public Nebulae::State, public Nebulae::InputListener
{
public:
  friend class ClientInputListener;

public:
  enum PlayerAction
  { 
    INPUT_ACTION_LEFT,
    INPUT_ACTION_RIGHT,
    INPUT_ACTION_UP,
    INPUT_ACTION_DOWN,
  };

  struct KeyBinding
  {
    Nebulae::KeyCode key;
    PlayerAction     action;
  };

public:
  typedef std::shared_ptr<Nebulae::RenderSystem > RenderSystemPtr;
  typedef std::shared_ptr<Nebulae::Camera >       CameraPtr;

private:
  RenderSystemPtr m_pRenderSystem; ///< The rendering system.
  CameraPtr       m_pCamera;       ///< The camera for scene.

  std::thread     m_fetch_thread;
  
  std::vector<entity_t > entities;
  vector2_t m_lastDirection;

  std::unique_ptr<zmq::context_t > context;
  std::unique_ptr<zmq::socket_t >  localSocket;
  std::unique_ptr<zmq::socket_t >  serverSocket;
  long long m_lag;

  std::vector<KeyBinding >   m_inputBindings;
  std::vector<PlayerAction > m_activeActions;
  std::string  m_lastCommand;     ///< The last command that was sent to the server for this player.

public:
  /** \name Structors */ ///@{
  ClientState();
  virtual ~ClientState();
  //@}

  /** \name Mutators */ ///@{
  virtual void Enter( Nebulae::StateStack* caller );
  virtual void Exit( Nebulae::StateStack* caller );
  virtual void Update( float fDeltaTimeStep, Nebulae::StateStack* pCaller );
  virtual void Render() const;
  //@}

  /** \name Accessors */ ///@{
  CameraPtr GetCamera() const {
    return m_pCamera;
  }
  RenderSystemPtr GetRenderSystem() const {
    return m_pRenderSystem;
  }
//@}

private:
  void SetBinding( int variant );
  virtual void KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys ) override;
  virtual void KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys ) override;

  std::string GetCurrentCommand() const;
  vector2_t GetDirectionFromCommand( const std::string& cmd );
  void SendClientUpdate();
  
  bool TryServerUpdate();

  void SimulateStep( float fDeltaTimeStep );

}; //ClientState


#endif // __FLOCK_CLIENTSTATE_H__
