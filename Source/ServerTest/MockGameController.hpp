#ifndef __FLOCK_TEST_MOCKGAMECONTROLLER_H__
#define __FLOCK_TEST_MOCKGAMECONTROLLER_H__

#include <Server/GameController.hpp>
#include "gmock/gmock.h"


class MockGameController : public GameController
{
public:
  MockGameController( Model* model ) :
    GameController( model )
  {}

  MOCK_METHOD3( PrepareEntity, void( entity_t&, const vector4_t&, const rect_t& ) );
  MOCK_METHOD1( Update, void ( float ) );

  void ParentClass_Update( float elapsed ) {
    return GameController::Update( elapsed );
  }
};

#endif // __FLOCK_TEST_MOCKGAMECONTROLLER_H__