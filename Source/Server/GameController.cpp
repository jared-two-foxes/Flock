
#include "GameController.hpp"

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
  entity_t* player = m_model->CreateEntity();
  if( player )
  {
    player->position = vector2_t( 0, 0 );
    player->direction = vector2_t( 0, 0 );
    player->speed = 1.0f;
    player->colour = vector4_t( 0, 1.0f, 0, 1.0f );
    player->player = 1;
    
    m_players.push_back( player );  
  }

  return player;
}


void 
GameController::PrepareEntity( entity_t& entity, const vector4_t& colour, const rect_t& zone )
{
  entity.position.x = RandFloat( zone.a.x, zone.b.x );
  entity.position.y = RandFloat( zone.a.y, zone.b.y );
  entity.colour = colour;
  entity.speed = 1.0f;
  entity.player = 0;
}


void 
GameController::Update( float delta )
{
  if( m_players.size() > 0 )
  {
    m_secondsToNextSpawn -= duration<float >( delta );

    if ( m_secondsToNextSpawn <= duration<float>::zero() )
    {
      entity_t* e = m_model->CreateEntity();
      PrepareEntity( *e, vector4_t(1.0,0.0f, 0.0f, 1.0f ), m_zone );
      m_secondsToNextSpawn += duration<float >( SPAWN_INTERVAL );
    }
  }
}