//
//  Weather update server in C++
//  Binds PUB socket to tcp://*:5556
//  Publishes random weather updates
//
#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>  // was included in <zhelpers.hpp> 

#include <Common/Math/Math.hpp>
#include <Common/Entity/Entity.hpp>
#include <Common/Platform/Console.hpp>
#include <Common/Platform/StopWatch.hpp>

#include <Server/Flocking.hpp>


void PrepareEntities( std::vector<entity_t >& entities, const box_t& zone )
{
  int identifier = 0;
  for ( entity_t& entity : entities )
  {
    entity.identifier = identifier++;
    entity.position.x = RandFloat( zone.min.x, zone.max.x );
    entity.position.y = RandFloat( zone.min.y, zone.max.y );
    entity.position.z = RandFloat( zone.min.z, zone.max.z );
    entity.position.w = RandFloat( zone.min.w, zone.max.w );;
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

	// Prepare the active play area for the simulation
	box_t zone;
	zone.min = vector4_t( -100.0f, -100.0f, -100.0f, 0 );
	zone.max = vector4_t(  100.0f,  100.0f,  100.0f, 0 );

  // Create our entities for simulation
	std::vector<entity_t > entities( argc > 1 ? atoi( argv[1] ) : 256 );
	PrepareEntities( entities, zone );

  plane_t planes[6];
  planes[0] = plane_t( vector4_t( 1.0f, 0.0f, 0.0f, 0.0f ), zone.min.x ); // left;
  planes[1] = plane_t( vector4_t( 0.0f, 1.0f, 0.0f, 0.0f ), zone.min.y ); // bottom;
  planes[2] = plane_t( vector4_t( 0.0f, 0.0f, 1.0f, 0.0f ), zone.min.z ); // far;
  planes[2] = plane_t( vector4_t( 1.0f, 0.0f, 0.0f, 0.0f ), zone.max.x ); // right;
  planes[2] = plane_t( vector4_t( 0.0f, 1.0f, 0.0f, 0.0f ), zone.max.y ); // top;
  planes[2] = plane_t( vector4_t( 0.0f, 0.0f, 1.0f, 0.0f ), zone.max.z ); // near;

	// GO! Simulate and push everything to anyone listening as fast as possible!
	while (1) 
	{
		// Start timer
		frameTimer.Start();

		// Update flocking simulation
		for( entity_t& entity : entities ) 
		{
			Attraction( entity, entities );
			Seperation( entity, entities ); 
			//@todo apply a cohesion force
		}

		//@todo Check that none of the entities are moving outside of the box.
    for( entity_t& entity : entities ) 
    {
      // Check this entity's proximity to each "wall" of the cage
      for( int i = 0; i < 6; ++i ) 
      {
        const float PROXIMITY_THRESHOLD = 1.0f;

        float distance = Distance( planes[i], entity.position );
        if( distance < PROXIMITY_THRESHOLD )
        {
          // Try to push the entity away from the wall.  Note that if the accumlated force  
          // moving the entity towards the wall is greater than the amount we try to move him away
          // it's going to simply keep moving in this direction.
          entity.position = entity.position + planes[i].getNormal() * entity.speed; 
        }
      }
    }

    // Push all the entities out to anyone listening.
    std::size_t list_size = entities.size(); 
    zmq::message_t message( list_size * sizeof( entity_t ) + sizeof( int ) );
    char* ptr = (char*)message.data();
    memcpy( ptr, &list_size, sizeof( int ) );
    memcpy( ptr + sizeof( int ), &entities[0], list_size * sizeof( entity_t ) );
    publisher.send( message );

		// End timer.
		float frame_time = frameTimer.Stop();

		// Update display with stats.
		Console::SetCursorPosition( initial_console_info.dwCursorPosition );

  	std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "Frame Time: " << frame_time << std::endl;
		std::cout << "Entities: " << entities.size() << std::endl;

		Console::GetScreenBufferInfo( &final_console_info );
	}

  Console::SetCursorVisible( true );
  Console::SetCursorPosition( final_console_info.dwCursorPosition );

	return 0;
}