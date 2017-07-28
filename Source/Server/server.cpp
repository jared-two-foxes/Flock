
#include "Server.hpp"

#include <Common/Platform/Console.hpp>

#include <boost/tokenizer.hpp>

#include <iostream>  // was included in <zhelpers.hpp> 
#include <sstream>

using namespace std::chrono;

std::chrono::high_resolution_clock::time_point _last, _open;
float _frame_time;

std::chrono::milliseconds Server::PUBLISH_FREQUENCY = ( 1000ms / 21 ); //< 21 ticks per second.


Server::Server( Model* model, GameController* controller ) :
  m_context( std::make_unique<zmq::context_t >( 1 ) ),
  m_publisher( std::make_unique<zmq::socket_t >( *m_context, ZMQ_PUB ) ),
  m_listener( std::make_unique<zmq::socket_t >( *m_context, ZMQ_REP ) ),
  m_model( model ),
  m_controller( controller )
{}


Server::~Server()
{}

void
Server::Init( const char* publisherEndPoint, const char* listenerEndPoint )
{
  console = Create();
  SetCursorVisible( console, false );

  m_publisher->bind( publisherEndPoint );
  m_listener->bind( listenerEndPoint );

  // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
  int conflate = 1;
  m_publisher->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );

  _open = _last = high_resolution_clock::now();
}


void
Server::PrintToConsole()
{
  // First clear the current contents
  COORD tl = { 0, 0 };
  SetCursorPosition( console, tl );

  std::cout << "Server " << "-v 1.0.0" << std::endl; //maybe add in IP address or something?
  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << "Frame Time  (ms): " << _frame_time << std::endl;
  std::cout << "Uptime (s): " << duration<float >( high_resolution_clock::now() - _open ).count() << std::endl;
  std::cout << "Players: " << m_controller->GetPlayers().size(); // << std::endl;
  for ( auto i : m_controller->GetPlayers() )
  {
    entity_t* p = m_model->Get( i );
    assert( p );
    std::cout << " {" << p->position.x << ", " << p->position.y << "}, ";
  }
  std::cout << std::endl;
  std::cout << "Entities: " << m_model->Entities().size() << std::endl;
  std::cout << "Timestamp: " << _last.time_since_epoch().count() << std::endl;
}


std::chrono::high_resolution_clock::time_point
Server::Update( std::chrono::high_resolution_clock::time_point last )
{
  // Determine elapsed time since last iteration.
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now - last;
  _frame_time = std::chrono::duration<float, std::milli>( duration ).count();

  // Check for any Client messages.
  zmq::message_t request;
  if ( m_listener->recv( &request, ZMQ_NOBLOCK ) )
  {
    std::string replyMsg = ProcessClientMessage( request );

    zmq::message_t reply( replyMsg.size() );
    memcpy( reply.data(), replyMsg.c_str(), replyMsg.size());
    m_listener->send( reply );
  }

  // Push the current state out to all of the subscribers
  while ( ( now - last ) >= PUBLISH_FREQUENCY )
  {
    // Update the game state.
    m_controller->Update( std::chrono::duration<float>( PUBLISH_FREQUENCY ).count() );

    
    long long nanoseconds_since_epoch = now.time_since_epoch().count();

    std::size_t list_size = m_model->Entities().size();
    zmq::message_t message( sizeof( long long ) + sizeof( int ) + list_size * sizeof( entity_t ) );
    char* ptr = ( char* ) message.data();
    memcpy( ptr, &nanoseconds_since_epoch, sizeof( long long ) );
    memcpy( ptr + sizeof( long long ), &list_size, sizeof( int ) );
    memcpy( ptr + sizeof( long long ) + sizeof( int ), &m_model->Entities()[ 0 ], list_size * sizeof( entity_t ) );
    m_publisher->send( message, ZMQ_NOBLOCK );

    last += PUBLISH_FREQUENCY;
  }

  _last = last;
  return last;
}

std::string
Server::ProcessClientMessage( zmq::message_t& request )
{
  static int s_nextClientID = 0;

  char buffer[256];
  memset( buffer, 0, 256 );
  //assert buffer size?
  memcpy( buffer, request.data(), request.size() );
  buffer[request.size()] = 0; //< null terminator.

  std::string str( buffer );
  boost::char_separator<char> sep( " " );
  boost::tokenizer<boost::char_separator<char> > tokens( str, sep );
 
  std::vector<std::string > tokenList;
  for ( auto it = tokens.begin(); it != tokens.end(); ++it )
  {
    tokenList.push_back( *it );
  }

  // Respond with a client-id.
  char replyBuffer[ 128 ];
  memset( replyBuffer, 0, 128 );

  if ( tokenList[ 0 ] == "join" )
  {
    snprintf( ( char * ) replyBuffer, 6, "%05d", s_nextClientID++ );
    replyBuffer[ 6 ] = 0;
  }
  else if ( tokenList[ 0 ] == "create" )
  {
    // Extract the number of players to create.
    int n = atoi( tokenList[ 1 ].c_str() );

    // Create a new player entity.
    std::vector<int > identifiers;
    for ( int i = 0; i < n; ++i )
    {
      entity_t* player = m_controller->AddPlayer();
      assert( player != nullptr );
      identifiers.push_back( player->identifier );
    }

    if ( !identifiers.empty() )
    {
      std::string msg( "OK" );
      for ( int id : identifiers )
      {
        msg += " ";
        msg += std::to_string( id );
      }
      memcpy( replyBuffer, msg.c_str(), msg.length() );
      replyBuffer[msg.length()] = 0;
    }
    else
    {
      //@todo - return failure?
    }
  }
  else
  {
    int clientId = atoi( tokenList[ 0 ].c_str() );;

    // Extract the command & identifier.
    if( tokenList.size() > 1 )
    {
      for ( std::size_t i = 1, n = tokenList.size(); i < n; )
      {
        int identifier = atoi( tokenList[ i++ ].c_str() );

        // Find the entity.
        entity_t* e = m_model->Get( identifier );
        if ( e )
        {
          vector2_t d( 0, 0 );
          if( i < n )
          {
            if ( strstr( tokenList[ i ].c_str(), "left" ) != nullptr )
            {
              d.x -= 1.0f;
            }
            if ( strstr( tokenList[ i ].c_str(), "right" ) != nullptr )
            {
              d.x += 1.0f;
            }
            if ( strstr( tokenList[ i ].c_str(), "up" ) != nullptr )
            {
              d.y += 1.0f;
            }
            if ( strstr( tokenList[ i ].c_str(), "down" ) != nullptr )
            {
              d.y -= 1.0f;
            }
          }

          if ( d.x > 0 || d.y > 0 )
          {
            d = Normalize( d );
          }

          e->direction = d;
        }

        i++;
      }
    }

    // Send reply back to client
    memcpy( replyBuffer, "OK", 3 );
    replyBuffer[3] = 0;
  }

  return replyBuffer;
}