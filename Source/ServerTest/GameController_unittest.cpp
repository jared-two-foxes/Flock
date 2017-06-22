
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

TEST( GameController, Update_PlayersConnected_SpawnedEntityRandomlyPositioned )
{
  Model model;
  MockGameController game( &model );
  game.AddPlayer();

  EXPECT_CALL( game, PrepareEntity( _, _, _ ) );

  auto dt = duration<float >( GameController::SPAWN_INTERVAL );
  game.ParentClass_Update( dt.count() );
}