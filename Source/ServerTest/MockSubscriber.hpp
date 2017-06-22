#ifndef __FLOCK_TEST_MOCKSUBSCRIBER_H__
#define __FLOCK_TEST_MOCKSUBSCRIBER_H__

#include "gtest/gtest.h"

#include <zmq.hpp>


struct MockSubscriber
{
public:
  zmq::context_t* m_context;
  std::unique_ptr<zmq::socket_t > m_subscriber;

public:
  MockSubscriber( zmq::context_t* ctx ) :
    m_context( ctx ),
    m_subscriber( std::make_unique<zmq::socket_t >( *m_context, ZMQ_SUB ) ) 
  {
    const char* filter = "";
    m_subscriber->connect( "tcp://localhost:5556" ); 
    m_subscriber->setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );
  }

  int Recv( zmq::message_t* message )
  {
    return m_subscriber->recv( message, ZMQ_DONTWAIT );
  }

};


#endif // #ifndef __FLOCK_TEST_MOCKSUBSCRIBER_H__