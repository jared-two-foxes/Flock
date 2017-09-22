
#include "GameController.hpp"

#include <Common/Entity/Flocking.hpp>

#include <assert.h>

using namespace std::chrono;
using namespace std::chrono_literals;


const std::chrono::duration<float > GameController::SPAWN_INTERVAL( 2s );


entity_t* FindClosestEntity( Model* model, entity_t* e )
{
  entity_t* t = nullptr;

  float d = std::numeric_limits<float >::max();
  for ( auto& o : model->Entities() )
  {
    if ( &o != e ) 
    {
      float m = LengthSq( o.position - e->position );
      if ( m < d )
      {
        t = &o;
        d = m;
      }
    }
  }

  return t;
}


struct Job
{
  virtual void operator()( Model* model, float delta ) = 0;
};


struct SpawnJob : public Job
{
  rect_t m_zone;
  std::chrono::duration<float > m_secondsToNextSpawn;

  void PrepareEntity( entity_t& entity, const rect_t& zone )
  {
    entity.radius = 2.5f;
    entity.position.x = RandFloat( zone.a.x, zone.b.x );
    entity.position.y = RandFloat( zone.a.y, zone.b.y );
    entity.speed = 2.0f;
    entity.player = false;
  }

  void operator()( Model* model, float delta )
  {
    m_secondsToNextSpawn -= duration<float >( delta );

    if ( m_secondsToNextSpawn <= duration<float >::zero() )
    {
      entity_t* e = model->CreateEntity();
      PrepareEntity( *e, m_zone );
      m_secondsToNextSpawn += duration<float >( GameController::SPAWN_INTERVAL );
    }
  }
};


struct EntityMotionJob : public Job
///
/// Controls how all entities that are not flagged as players will move this frame.
///
{
  void operator()( Model* model, float delta )
  {
    // Create the players list.
    auto entities = model->Entities();

    // Update all enemy entities direction such that move towards the closest entity if they are larger than it
    // or away from it if they are smaller. 
    for ( auto& e : entities )
    {
      if ( !e.player )
      {
        entity_t* t = FindClosestEntity( model, &e );
        // Assume that we found one.
        //NE_ASSERT( t != nullptr, "Unable to find a player?" )();

        if ( e.radius > t->radius )
        {
          Attraction( e, *t );
        }
      }
    }
  }
};


struct MoveEntitiesJob : public Job
{
  void operator()( Model* model, float delta ) 
  {
    // Move all the entities based upon their speed based upon their movement direction.
    for ( auto& e : model->Entities() )
    {
      e.position = e.position + e.direction * e.speed * delta;
    }
  }
};


struct CombinationJob : public Job
{
  void Eat( entity_t* large, entity_t* small )
  {
    large->radius += small->radius;
    //small->dead = true;
  }

  void operator()( Model* model, float delta )
  {
    // Check for any overlapped entities and eliminate the smaller entities and increase the size of the smaller entity.
    
    for ( auto& e : model->Entities() ) 
    {
      entity_t* o = FindClosestEntity( model, &e );
      
      // Check if they overlap.
      vector2_t d = (*o).position - e.position;
      if ( Length( d ) < ( e.radius + o->radius ) )
      {
        // The large eats the small.
        Eat( e.radius > o->radius ? &e : o, e.radius > o->radius ? o : &e );
      }
    }
  }
};


GameController::GameController( Model* model ) :
  m_model( model ),
  m_zone( vector2_t( -100.0f, -100.0f ), vector2_t( 100.0f, 100.0f ) ),
  m_secondsToNextSpawn( SPAWN_INTERVAL )
{}


void
GameController::Init()
{
  SpawnJob* spawn_job = new SpawnJob();
  spawn_job->m_zone = m_zone;
  spawn_job->m_secondsToNextSpawn = GameController::SPAWN_INTERVAL;
  m_jobs.push_back( spawn_job );

  EntityMotionJob* behaviour_job = new EntityMotionJob();
  m_jobs.push_back( behaviour_job );

  MoveEntitiesJob* movement_job = new MoveEntitiesJob();
  m_jobs.push_back( movement_job );

  CombinationJob* combination_job = new CombinationJob();
  m_jobs.push_back( combination_job );
}

entity_t*
GameController::AddPlayer()
{
  static vector2_t spawn_points[] = { {-75.0f,-75.0f}, {75.0f,75.0f}, {-75.0f,75.0f}, {75.0f,-75.0f} };

  entity_t* player = m_model->CreateEntity();
  if ( player )
  {
    player->radius = 2.5f;
    player->position = spawn_points[ m_players.size() ];
    player->direction = vector2_t( 0, 0 );
    player->speed = 10.0f;
    player->player = true;

    m_players.push_back( player->identifier );
  }

  return player;
}


void
GameController::Update( float delta )
{
  if ( m_players.size() > 0 )
  {
    for ( Job* job : m_jobs )
    {
      ( *job )( m_model, delta );
    }



    // Move all the entities based upon their speed based upon their movement direction.
    for ( auto& e : m_model->Entities() )
    {
      e.position = e.position + e.direction * e.speed * delta;
    }
  }
}

