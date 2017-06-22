
#include "Server.hpp"

#include <Common/Platform/Console.hpp>


#include <iostream>  // was included in <zhelpers.hpp> 
#include <sstream>

using namespace std::chrono;

std::chrono::high_resolution_clock::time_point _last, _open, _next;
float _frame_time;
CONSOLE_SCREEN_BUFFER_INFO _initial_console_info, _final_console_info;

std::chrono::milliseconds PUBLISH_FREQUENCY = 30ms;

Server::Server( Model* model, GameController* controller ) :
  m_context( std::make_unique<zmq::context_t >( 1 ) ),
  m_publisher( std::make_unique<zmq::socket_t >( *m_context, ZMQ_PUB ) ),
  m_listener( std::make_unique<zmq::socket_t >( *m_context, ZMQ_REP ) ),
  m_model( model ),
  m_controller( controller )
{}


Server::~Server()
{
  // This COMPLETELY fucks with unittest output!
  //Console::SetCursorVisible( true );
  //Console::SetCursorPosition( _final_console_info.dwCursorPosition );
}

void
Server::Init( const char* publisherEndPoint, const char* listenerEndPoint )
{
  Console::Init();
  Console::SetCursorVisible( false );
  Console::GetScreenBufferInfo( &_initial_console_info );

  m_publisher->bind( publisherEndPoint );
  m_listener->bind( listenerEndPoint );

  // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
  int conflate = 1;
  m_publisher->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );

  _open = _last = high_resolution_clock::now();
  _next = _open;
}


void
Server::PrintToConsole()
{
  Console::SetCursorPosition( _initial_console_info.dwCursorPosition );

  std::cout << "--------------------------------------------------------" << std::endl;
  std::cout << "Frame Time  (ms): " << _frame_time * 1000.0f << std::endl;
  std::cout << "Uptime (s): " << duration<float >( high_resolution_clock::now() - _open ).count() << std::endl;
  //std::cout << "Players: " << players.size() << std::endl;
  std::cout << "Entities: " << m_model->Entities().size() << std::endl;
  std::cout << "Timestamp: " << _last.time_since_epoch().count() << std::endl;

  //Console::GetScreenBufferInfo( &_final_console_info );
}


void
Server::Update()
{
  // Determine elapsed time since last iteration.
  auto now = high_resolution_clock::now();
  duration<float > duration = ( now - _last );
  _frame_time = duration.count();

  // Update the game state.
  m_controller->Update( _frame_time );


  // Check for anu Client messages.
  zmq::message_t request;
  if ( m_listener->recv( &request, ZMQ_NOBLOCK ) )
  {
    zmq::message_t reply( 32 );
    ProcessClientMessage( request, &reply );

    m_listener->send( reply );
  }

  // Push the current state out to all of the subscribers
  if( now >= _next )
  {
    long long nanoseconds_since_epoch = now.time_since_epoch().count();

    std::size_t list_size = m_model->Entities().size();
    zmq::message_t message( sizeof( long long ) + sizeof( int ) + list_size * sizeof( entity_t ) );
    char* ptr = ( char* ) message.data();
    memcpy( ptr, &nanoseconds_since_epoch, sizeof( long long ) );
    memcpy( ptr + sizeof( long long ), &list_size, sizeof( int ) );
    memcpy( ptr + sizeof( long long ) + sizeof( int ), &m_model->Entities()[ 0 ], list_size * sizeof( entity_t ) );
    m_publisher->send( message, ZMQ_NOBLOCK );

    _next += PUBLISH_FREQUENCY;
  }

  _last = now;
}

void
Server::ProcessClientMessage( zmq::message_t& request, zmq::message_t* reply )
{
  // //  Do some 'work'
  std::istringstream iss( static_cast< char* >( request.data() ) );

  if ( iss.str() == "new" )
  {
    // Create a new player entity.
    entity_t* player = m_controller->AddPlayer();

    // Send reply back to client of the id that this player will be.
    snprintf( ( char * ) reply->data(), 6,
      "%05d", player->identifier );
  }
  else
  {
    // Extract the command & identifier.
    int identifier;
    std::string command;
    iss >> identifier >> command;

    // Find the entity.
    entity_t* e = m_model->Get( identifier );
    if ( e )
    {
      if ( command == "left" )
        e->position.x -= e->speed;
      else if ( command == "right" )
        e->position.x += e->speed;
      else if ( command == "up" )
        e->position.y += e->speed;
      else if ( command == "down" )
        e->position.y -= e->speed;

      // Send reply back to client
      memcpy( ( char * ) reply->data(), "OK", 3 );
    }
    else
    {
      // Send reply back to client
      memcpy( ( char * ) reply->data(), "INVALID_ID", 11 );
    }
  }
}