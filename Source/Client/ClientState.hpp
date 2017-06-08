#ifndef __NEBULAE_GUISAMPLESTATE_H__
#define __NEBULAE_GUISAMPLESTATE_H__


#include "Common/Entity/Entity.hpp"

#include <Nebulae/Beta/StateStack/State.h>
#include <Nebulae/Common/Common.h>

#include <zmq.hpp>


namespace Nebulae {
  class Camera;
  class RenderSystem;
}


/** ClientState.
 */
class ClientState : public Nebulae::State
{
public:
  typedef std::shared_ptr<Nebulae::RenderSystem > RenderSystemPtr;
  typedef std::shared_ptr<Nebulae::Camera >       CameraPtr;

private:
  RenderSystemPtr m_pRenderSystem; ///< The rendering system.
  CameraPtr       m_pCamera;       ///< The camera for scene.

  std::vector<entity_t > entities;

  std::unique_ptr<zmq::context_t > context;
  std::unique_ptr<zmq::socket_t >  subscriber;

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
    CameraPtr         GetCamera() const         { return m_pCamera; }
    RenderSystemPtr   GetRenderSystem() const   { return m_pRenderSystem; }
    //@}

}; //ClientState


#endif // __NEBULAE_GUISAMPLESTATE_H__
