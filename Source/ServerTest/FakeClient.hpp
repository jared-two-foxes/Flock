#ifndef __FLOCK_TEST_FAKECLIENT_H__
#define __FLOCK_TEST_FAKECLIENT_H__

#include <zmq.hpp>


struct FakeClient
{
public:
  zmq::socket_t _socket;

public:
  FakeClient( zmq::context_t& context ) :
    _socket( context, ZMQ_REQ )
  {}

  void
  connect( const char* endpoint )
  {
    _socket.connect( endpoint );  
  }

  std::string 
  pushClientMessage( Server& server, const std::string& message )
  {
    // Send the connect request.
    zmq::message_t request( message.length() + 1 );
    snprintf( ( char* ) request.data(), message.length() + 1, message.c_str() );
    _socket.send( request );

    // Grab the reply.
    zmq::message_t reply;
    while ( !_socket.recv( &reply, ZMQ_NOBLOCK ) )
    {
      // Let the Server catch it.
      server.Update();
    }

    return reply.data<char>();
  }

};

#endif // __FLOCK_TEST_FAKECLIENT_H__