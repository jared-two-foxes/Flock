#ifndef _FLOCK_SERVER_HPP_
#define _FLOCK_SERVER_HPP_

#include <Server/GameController.hpp>

#include <Common/Model/Model.hpp>

#include <zmq.hpp>

#include <memory>
#include <vector>

class Server
{
public:
  std::unique_ptr<zmq::context_t > m_context;
  std::unique_ptr<zmq::socket_t >  m_publisher;
  std::unique_ptr<zmq::socket_t >  m_listener;
  
  Model* m_model;
  GameController* m_controller;

public:
  Server( Model* model, GameController* controller );
  virtual ~Server();

  zmq::context_t* _context() { return m_context.get(); }
  zmq::socket_t*  _publisher() { return m_publisher.get(); }
  zmq::socket_t*  _listener() { return m_listener.get(); }

  void Init( const char* publisherEndPoint, const char* listenerEndPoint );
  void PrintToConsole();
  void Update();

  void ProcessClientMessage( zmq::message_t& request, zmq::message_t* reply );
};

#endif // _FLOCK_SERVER_HPP_