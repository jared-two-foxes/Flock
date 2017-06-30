#ifndef __FLOCK_CLIENTSTATE_H__
#define __FLOCK_CLIENTSTATE_H__


#include "Common/Entity/Entity.hpp"

#include <Nebulae/Beta/StateStack/State.h>
#include <Nebulae/Common/Common.h>

#include <zmq.hpp>


namespace Nebulae {
  class Camera;
  class RenderSystem;
}

class ClientInputListener;

/** ClientState.
 */
class ClientState : public Nebulae::State
{
public:
  friend class ClientInputListener;

public:
  typedef std::shared_ptr<Nebulae::RenderSystem > RenderSystemPtr;
  typedef std::shared_ptr<Nebulae::Camera >       CameraPtr;

private:
  RenderSystemPtr m_pRenderSystem; ///< The rendering system.
  CameraPtr       m_pCamera;       ///< The camera for scene.

  std::unique_ptr<ClientInputListener > m_pInputListener;

  std::vector<entity_t > entities;

  std::unique_ptr<zmq::context_t > context;
  std::unique_ptr<zmq::socket_t >  localSocket;
  std::unique_ptr<zmq::socket_t >  serverSocket;
  long long m_lag;

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
  Nebulae::KeyCode pressedKey;
  void OnKeyDown( Nebulae::KeyCode keyCode );
  void OnKeyUp( Nebulae::KeyCode keyCode );

  void SendClientUpdate();
  void TryServerUpdate();

}; //ClientState


#endif // __FLOCK_CLIENTSTATE_H__
