
#include "Server/Server.hpp"

#include "gtest/gtest.h"
#include <zmq.hpp>

#include "MockGameController.hpp"
#include "MockModel.hpp"
#include "MockSubscriber.hpp"
#include "FakeClient.hpp"


using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

const char* PUBLISHER_ENDPOINT = "tcp://*:5556";
const char* REQUEST_ENDPOINT = "tcp://*:5555";



TEST( Server, Ctor_CreatesAndBindsPublisherSocket )
{
  Model model;
  GameController controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );

  std::size_t len = 256;
  char endpoint[ 256 ];
  server._publisher()->getsockopt( ZMQ_LAST_ENDPOINT, (void*)&endpoint[0], &len );

  EXPECT_STREQ( endpoint, "tcp://0.0.0.0:5556" );
}

TEST( Server, Ctor_CreatesAndBindsListenerSocket )
{
  Model model;
  GameController controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );

  std::size_t len = 256;
  char endpoint[ 256 ];
  server._listener()->getsockopt( ZMQ_LAST_ENDPOINT, ( void* ) &endpoint[ 0 ], &len );

  EXPECT_STREQ( endpoint, "tcp://0.0.0.0:5555" );
}

TEST( Server, Update_BroadcastsState )
{
  Model model;
  model.CreateEntity();
  GameController controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );
  MockSubscriber subscriber( server._context() );

  // zmq subscriber pattern will drop a couple of "frames" when first connecting due to the "handshaking" that occurs when
  // first making the connection, so discard these and advance to the point were we actually have a valid connection.
  zmq::message_t message;
  while( message.size() <= 0 ) {
    server.Update();
    int rc = subscriber.Recv( &message );
    EXPECT_NE( rc, -1 );
  }

  int list_size;
  std::vector<entity_t > entities;
  char* data_ptr = ( char* ) message.data();
  memcpy( &list_size, data_ptr + sizeof( long long ), sizeof( int ) ); //< advance by sizeof( long long ) to skip the timestamp
  entities.resize( list_size );
  memcpy( &entities[ 0 ], data_ptr + sizeof( long long ) + sizeof( int ), list_size * sizeof( entity_t ) ); //< Smash over the top all entities with info from server.
  EXPECT_EQ( entities.size(), 1 );
  EXPECT_EQ( memcmp( &model.Entities()[0], &entities[0], sizeof(entity_t) ), 0 );
}

TEST( Server, Update_UpdatesGameController )
{
  Model model;
  MockGameController controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );

  EXPECT_CALL( controller, Update(_) );
  
  server.Update();
}

TEST( Server, PlayerInput_Connect_CreatesPlayerEntity )
{
  entity_t e;
  NiceMock<MockModel > model;
  NiceMock<MockGameController > controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );
  FakeClient client( *server._context() );
  client.connect( "tcp://localhost:5555" );

  EXPECT_CALL( model, CreateEntity() )
    .WillOnce( Return( &e ) );

  client.pushClientMessage( server, "new" );

  EXPECT_THAT( e.player, 1 );
}

TEST( Server, PlayerInput_Connect_RepliesWithIdentifier )
{
  entity_t e;
  e.identifier = 1;
  NiceMock<MockModel > model;
  NiceMock<MockGameController > controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );
  FakeClient client( *server._context() );
  client.connect( "tcp://localhost:5555" );

  ON_CALL( model, CreateEntity() )
    .WillByDefault( Return( &e ) );

  std::string reply = client.pushClientMessage( server, "new" );

  EXPECT_STREQ( reply.c_str(), "00001" );
}


TEST( Server, PlayerInput_Update_SetsPlayersPosition )
{
  Model model;
  NiceMock<MockGameController > controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );
  FakeClient client( *server._context() );
  client.connect( "tcp://localhost:5555" );

  std::string reply = client.pushClientMessage( server, "new" );
  entity_t* e = model.Get( atoi( reply.c_str() ) );
  ASSERT_NE( e, nullptr );
  vector2_t original = e->position;
  client.pushClientMessage( server, std::to_string( e->identifier ) + " left" );

  EXPECT_LT( e->position.x, original.x );
}

TEST( Server, PlayerInput_Update_ValidArgsReturnsSuccess )
{
  Model model;
  NiceMock<MockGameController > controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );
  FakeClient client( *server._context() );
  client.connect( "tcp://localhost:5555" );

  std::string reply;
  reply = client.pushClientMessage( server, "new" );
  entity_t* e = model.Get( atoi( reply.c_str() ) );
  ASSERT_NE( e, nullptr ); 
  reply = client.pushClientMessage( server, std::to_string( e->identifier ) + " left" );

  EXPECT_STREQ( reply.c_str(), "OK" );
}


TEST( Server, PlayerInput_Update_InvalidArgsReturnsError )
{
  Model model;
  NiceMock<MockGameController > controller( &model );
  Server server( &model, &controller );
  server.Init( PUBLISHER_ENDPOINT, REQUEST_ENDPOINT );
  FakeClient client( *server._context() );
  client.connect( "tcp://localhost:5555" );

  std::string reply;
  reply = client.pushClientMessage( server, "00001 left" );

  EXPECT_STREQ( reply.c_str(), "INVALID_ID" );
}