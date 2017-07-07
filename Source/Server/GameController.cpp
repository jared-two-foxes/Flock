
#include "GameController.hpp"

#include <Common/Entity/Flocking.hpp>

#include <assert.h>

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
    player->speed = 3.0f;
    player->player = true;

    m_players.push_back( player->identifier );
  }

  return player;
}


void
GameController::PrepareEntity( entity_t& entity, const rect_t& zone )
{
  entity.radius = 2.5f;
  entity.position.x = RandFloat( zone.a.x, zone.b.x );
  entity.position.y = RandFloat( zone.a.y, zone.b.y );
  entity.speed = 2.0f;
  entity.player = false; 
}


void
GameController::Update( float delta )
{
  if ( m_players.size() > 0 )
  {
    m_secondsToNextSpawn -= duration<float >( delta );

    if ( m_secondsToNextSpawn <= duration<float >::zero() )
    {
      entity_t* e = m_model->CreateEntity();
      PrepareEntity( *e, m_zone );
      m_secondsToNextSpawn += duration<float >( SPAWN_INTERVAL );
    }

    // Update all enemy entities direction such that movement will move them towards the
    // nearest player
    for ( auto& e : m_model->Entities() )
    {
      if ( !e.player )
      {
        float d = std::numeric_limits<float >::max();
        entity_t* t = nullptr;

        for( auto i : m_players )
        {
          entity_t* p = m_model->Get( i );
          assert( p );
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

    // Move all the entities based upon their speed based upon their movement direction.
    for ( auto& e : m_model->Entities() )
    {
      e.position = e.position + e.direction * e.speed * delta;
    }
  }
}