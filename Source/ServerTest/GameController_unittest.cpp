
#include "Server/GameController.hpp"

#include "gtest/gtest.h"

#include <chrono>


#include "MockModel.hpp"
#include "MockGameController.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

const float ENTITY_RADIUS = 2.5f;
const float PLAYER_RADIUS = 2.5f;

const float CMP_EPS       = 0.001f;

TEST( GameController, Update_NoPlayers_DoesntSpawnsEntities )
{
  entity_t e;
  MockModel model;
  GameController game( &model );

  ON_CALL( model, CreateEntity() )
    .WillByDefault( Return( &e ) );

  EXPECT_CALL( model, CreateEntity() )
    .Times( 0 );

  auto dt = duration<float >( GameController::SPAWN_INTERVAL );
  game.Update( dt.count() );
}

TEST( GameController, Update_PlayersConnected_MovesEntities )
{
  Model model;
  GameController game( &model );
  entity_t* p = game.AddPlayer();
  ASSERT_NE( nullptr, p );
  p->direction = vector2_t( 1.0f, 0.0f ); //unit vector to the right
  
  vector2_t start = p->position;
  float     speed = p->speed;
  game.Update( 1.0f ); //< 1 second.

  EXPECT_EQ( Length( start - p->position ), speed );
}


TEST( GameController, Update_PlayersConnected_SpawnsEntityPeriodically )
{
  entity_t player, e;
  NiceMock<MockModel > model;
  GameController game( &model );
  
  ON_CALL( model, CreateEntity() )
    .WillByDefault( Return( &player ) );

  game.AddPlayer();
  
  EXPECT_CALL( model, CreateEntity() )
    .WillOnce( Return( &e ) );

  auto dt = duration<float >( GameController::SPAWN_INTERVAL );
  game.Update( dt.count() );
}

TEST( GameController, Update_PlayersConnected_PreparesSpawnedEntities )
{
  Model model;
  MockGameController game( &model );
  game.AddPlayer();

  EXPECT_CALL( game, PrepareEntity( _, _ ) );

  auto dt = duration<float >( GameController::SPAWN_INTERVAL );
  game.ParentClass_Update( dt.count() );
}

TEST( GameController, Update_PlayersConnected_EntitiesAttractedToPlayer )
{
  Model model;
  GameController game( &model );
  entity_t* p = game.AddPlayer();
  rect_t zone( vector2_t( -100.0f, -100.0f ), vector2_t( 100.0f, 100.0f ) );
  entity_t* e = model.CreateEntity();
  game.PrepareEntity( *e, zone );
  vector2_t start = e->position;
  vector2_t player = p->position;
  float d = LengthSq( player - start );
  auto dt = duration<float >( GameController::SPAWN_INTERVAL );
  game.Update( 0 );

  EXPECT_LE( LengthSq( player - e->position ), d );
}

TEST( GameController, PrepareEntity_SpawnedEntityRandomlyPositioned )
{
  Model model;
  entity_t *a, *b;
  GameController game( &model );
  a = model.CreateEntity(); b = model.CreateEntity();
  rect_t zone( vector2_t( -100.0f, -100.0f ), vector2_t( 100.0f, 100.0f ) );

  game.PrepareEntity( *a, zone );
  game.PrepareEntity( *b, zone );

  EXPECT_GE( fabs( a->position.x - b->position.x ), 0.01f );
}

TEST( GameController, PrepareEntity_SetsEntitiesRadius )
{
  Model model;
  GameController game( &model );
  entity_t* e = model.CreateEntity();
  rect_t zone( vector2_t( -100.0f, -100.0f ), vector2_t( 100.0f, 100.0f ) );

  game.PrepareEntity( *e, zone );

  EXPECT_LE( fabs( e->radius - ENTITY_RADIUS ), CMP_EPS );
}

TEST( GameController, AddPlayer_SetsPlayersRadius )
{
  Model model;
  GameController game( &model );
  
  entity_t* e = game.AddPlayer();

  EXPECT_LE( fabs( e->radius - PLAYER_RADIUS ), CMP_EPS );
}