
#include "GameController.hpp"

#include <Common/Entity/Flocking.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;


const std::chrono::duration<float > GameController::SPAWN_INTERVAL( 2s );



GameController::GameController( Model* model ) :
  m_model( model ),
  m_zone( vector2_t( -100.0f, -100.0f ), vector2_t( 100.0f, 100.0f ) ),
  m_secondsToNextSpawn( SPAWN_INTERVAL )
{}


entity_t*
GameController::AddPlayer()
{
  static vector2_t spawn_points[] = { {-75.0f,-75.0f}, {75.0f,75.0f}, {-75.0f,75.0f}, {75.0f,-75.0f} };

  entity_t* player = m_model->CreateEntity();
  if ( player )
  {
    player->radius = 2.5f;
    player->position = spawn_points[m_players.size()];
    player->direction = vector2_t( 0, 0 );
    player->speed = 0.5f;
    player->colour = vector4_t( 0, 1.0f, 0, 1.0f );
    player->player = true;

    m_players.push_back( player );
  }

  return player;
}


void
GameController::PrepareEntity( entity_t& entity, const vector4_t& colour, const rect_t& zone )
{
  entity.radius = 2.5f;
  entity.position.x = RandFloat( zone.a.x, zone.b.x );
  entity.position.y = RandFloat( zone.a.y, zone.b.y );
  entity.colour = colour;
  entity.speed = 0.01f;
  entity.player = false; 
}


void
GameController::Update( float delta )
{
  if ( m_players.size() > 0 )
  {
    m_secondsToNextSpawn -= duration<float >( delta );

    if ( m_secondsToNextSpawn <= duration<float>::zero() )
    {
      entity_t* e = m_model->CreateEntity();
      PrepareEntity( *e, vector4_t( 1.0, 0.0f, 0.0f, 1.0f ), m_zone );
      m_secondsToNextSpawn += duration<float >( SPAWN_INTERVAL );
    }

    //@todo - Move entities towards the closest entity only?
    for ( auto& e : m_model->Entities() )
    {
      if ( !e.player )
      {
        float d = std::numeric_limits<float >::max();
        entity_t* t = nullptr;

        for( auto& p : m_players )
        {
          float m = LengthSq( p->position - e.position );
          if( m < d )
          {
            t = p;
            d = m;
          }
        }

        // Assume that we found one.
        //NE_ASSERT( t != nullptr, "Unable to find a player?" )();
        Attraction( e, *t );
      }
    }

    //Attraction( , *m_players[ 0 ] );
  }
}