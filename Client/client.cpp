//
//  Weather update client in C++
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode
//
#include <zmq.hpp>
#include <iostream>
#include <sstream>

#include "Common/Entity/Entity.hpp"
#include "Common/Platform/Console.hpp"
#include "Common/Platform/StopWatch.hpp"


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nShowCmd )
{
  CONSOLE_SCREEN_BUFFER_INFO initial_console_info, final_console_info;

  StopWatch frameTimer;

  // Prepare the console.
  Console::Init();
  Console::SetCursorVisible( false );
  Console::GetScreenBufferInfo( &initial_console_info );

  // Prepare our context and subscriber socket
  zmq::context_t context( 1 );
  zmq::socket_t subscriber( context, ZMQ_SUB );
  subscriber.connect( "tcp://localhost:5556" );
  
  // Setting subscription to all events. Argument is not currently supported, server doesnt push a identifier yet.
  const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
  subscriber.setsockopt( ZMQ_SUBSCRIBE, filter, strlen(filter) );

  std::vector<entity_t > entities;

  // Begin main loop!
  std::cout << "Begin Loop" << std::endl;
  while( 1 )
  {
    // Start timer
    frameTimer.Start();

    // Attempt to pull a new update from publisher
    zmq::message_t update;
    subscriber.recv( &update );

    // Process retrieved data.
    int list_size;
    char* data_ptr = (char*)update.data();
    memcpy( &list_size, data_ptr, sizeof(int) );
    entities.resize( list_size );
    memcpy( &entities[0], data_ptr + sizeof( int ), list_size * sizeof( entity_t ) ); //< Smash over the top all entities with info from server.
    //ProcessUpdate( pushed_entities );

    // End timer.
    float frame_time = frameTimer.Stop();

    // Update display with stats.
    Console::SetCursorPosition( initial_console_info.dwCursorPosition );

    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "Frame Time: " << frame_time << std::endl;
    std::cout << "Entities: " << entities.size() << std::endl;

    Console::GetScreenBufferInfo( &final_console_info );
  }

  return 0;
}