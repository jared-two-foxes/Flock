#ifndef __FLOCK_CLIENTSTATE_H__
#define __FLOCK_CLIENTSTATE_H__


#include <Common/Entity/Entity.hpp>

#include <Nebulae/Beta/StateStack/State.h>
#include <Nebulae/Common/Common.h>

#include <Common/Platform/Console.hpp>

#include <zmq.hpp>

#include <thread>

namespace Nebulae {
  class Camera;
  class RenderSystem;
}

class PlayerController;

/** ClientState.
 */
class ClientState : public Nebulae::State, public Nebulae::InputListener
{
public:
  typedef std::shared_ptr<Nebulae::RenderSystem > RenderSystemPtr;
  typedef std::shared_ptr<Nebulae::Camera >       CameraPtr;

private:
  RenderSystemPtr m_pRenderSystem; ///< The rendering system.
  CameraPtr       m_pCamera;       ///< The camera for scene.

  console_t m_console;

  std::vector<entity_t > m_entities;
  std::vector<PlayerController* > m_players;  

  int m_clientId;
  std::unique_ptr<zmq::context_t > context;
  std::unique_ptr<zmq::socket_t >  subscriberSocket;
  std::unique_ptr<zmq::socket_t >  serverSocket;
  std::string m_lastCommand;
  std::thread m_fetch_thread; 
  long long m_lag;


public:
  ClientState();
  virtual ~ClientState();

  virtual void Enter( Nebulae::StateStack* caller );
  virtual void Exit( Nebulae::StateStack* caller );
  virtual void Update( float fDeltaTimeStep, Nebulae::StateStack* pCaller );
  virtual void KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys ) override;
  virtual void KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys ) override; 
  virtual void Render() const;

private:
  void SetupServerSocket();
  std::string CreateClientMessage();
  void ProcessClientMessage( const std::string& msg );
  void TryCreatePlayers( int count );
  void TrySendClientUpdate();
  bool TryServerUpdate();
  void SimulateStep( float fDeltaTimeStep );

  void PrintToConsole();

}; //ClientState


#endif // __FLOCK_CLIENTSTATE_H__
