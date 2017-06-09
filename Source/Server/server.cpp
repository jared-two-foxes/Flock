//
//  Weather update server in C++
//  Binds PUB socket to tcp://*:5556
//  Publishes random weather updates
//
#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>  // was included in <zhelpers.hpp> 
#include <sstream>

#include <Common/Math/Math.hpp>
#include <Common/Entity/Entity.hpp>
#include <Common/Platform/Console.hpp>
#include <Common/Platform/StopWatch.hpp>

#include <Server/Flocking.hpp>


void PrepareEntities( std::vector<entity_t >& entities, const vector4_t& colour, const rect_t& zone )
{
  int identifier = 0;
  for ( entity_t& entity : entities )
  {
    entity.identifier = identifier++;
    entity.position.x = RandFloat( zone.a.x, zone.b.x );
    entity.position.y = RandFloat( zone.a.y, zone.b.y );
    entity.colour = colour;
    entity.speed = 1.0f;
    entity.player = 0;
  }
}


int main( int argc, char* argv[] )
{
  CONSOLE_SCREEN_BUFFER_INFO initial_console_info, final_console_info;

  srand( time( NULL ) );
  StopWatch frameTimer;

  // Prepare the console.
  Console::Init();
  Console::SetCursorVisible( false );
  Console::GetScreenBufferInfo( &initial_console_info );

  // Prepare our context and publisher
  zmq::context_t context( 1 );
  zmq::socket_t  publisher( context, ZMQ_PUB );
  publisher.bind( "tcp://*:5556" );
#if !defined( WIN32 )
  publisher.bind( "ipc://weather.ipc" ); // Not usable on windows.
#endif
  zmq::socket_t  replySocket( context, ZMQ_REP );
  replySocket.bind( "tcp://*:5555" );

  // Prepare the active play area for the simulation
  rect_t zone;
  zone.a = vector2_t( -100.0f, -100.0f );
  zone.b = vector2_t( 100.0f, 100.0f );

  std::vector<entity_t* > players;

  // Create our entities for simulation
  std::vector<entity_t > entities( argc > 1 ? atoi( argv[ 1 ] ) : 256 );
  PrepareEntities( entities, vector4_t( 1.0f, .0f, .0f, 1.0f ), zone );

  // GO! Simulate and push everything to anyone listening as fast as possible!
  while ( 1 )
  {
    // Start timer
    frameTimer.Start();

    // Update flocking simulation
    for ( entity_t& entity : entities )
    {
      if ( entity.player == 0 ) // IsPlayer
      {
        for ( entity_t* player : players )
        {
          Attraction( entity, *player );
        }
      }
    }

    //@todo Check that none of the entities are moving outside of the box.
    //for( entity_t& entity : entities ) 
    //{
    //  // Check this entity's proximity to each "wall" of the cage
    //  for( int i = 0; i < 6; ++i ) 
    //  {
    //    float distance = Distance( planes[i], entity.position );
    //    if( distance < 0 )
    //    {
    //      // Dont let the entities escape from the tank.
    //      entity.position = entity.position - planes[i].getNormal() * distance;
    //    }
    //  }
    //}

    // Push all the entities out to anyone listening.
    {
      std::size_t list_size = entities.size();
      zmq::message_t message( list_size * sizeof( entity_t ) + sizeof( int ) );
      char* ptr = ( char* ) message.data();
      memcpy( ptr, &list_size, sizeof( int ) );
      memcpy( ptr + sizeof( int ), &entities[ 0 ], list_size * sizeof( entity_t ) );
      publisher.send( message );
    }

    // Check for any client messages to process.
    {
      zmq::message_t request;

      //  Wait for next request from client
      if ( replySocket.recv( &request, ZMQ_NOBLOCK ) )
      {
        //  Do some 'work'
        std::istringstream iss( static_cast< char* >( request.data() ) );

        if ( iss.str() == "new" )
        {
          // Create a new player entity.
          entities.push_back( entity_t() );
          entity_t& player = entities.back();
          player.identifier = entities.size() - 1;
          player.position = vector2_t( 0, 0 );
          player.direction = vector2_t( 0, 0 );
          player.speed = 1.0f;
          player.colour = vector4_t( 0, 1.0f, 0, 1.0f );
          player.player = 1;

          players.push_back( &player );

          // Send reply back to client of the id that this player will be.
          zmq::message_t reply( 8 );
          snprintf( ( char * ) reply.data(), 8,
            "%05d", player.identifier );
          replySocket.send( reply );
        }
        else
        {
          // Extract the command & identifier.
          int identifier;
          std::string command;
          iss >> identifier >> command;

          // Find the entity.
          entity_t& e = entities[ identifier ];
          if ( command == "left" )
            e.position.x -= e.speed;
          else if ( command == "right" )
            e.position.x += e.speed;
          else if ( command == "up" )
            e.position.y += e.speed;
          else if ( command == "down" )
            e.position.y -= e.speed;

          // Send reply back to client
          zmq::message_t reply( 20 );
          memcpy( reply.data(), "OK", 2 );
          replySocket.send( reply );
        }
      }
    }

    // End timer.
    float frame_time = frameTimer.Stop();

    // Update display with stats.
    {
      Console::SetCursorPosition( initial_console_info.dwCursorPosition );

      std::cout << "--------------------------------------------------------" << std::endl;
      std::cout << "Frame Time: " << frame_time << std::endl;
      std::cout << "Players: " << players.size() << std::endl;
      std::cout << "Entities: " << entities.size() << std::endl;

      Console::GetScreenBufferInfo( &final_console_info );
    }
  }

  Console::SetCursorVisible( true );
  Console::SetCursorPosition( final_console_info.dwCursorPosition );

  return 0;
}