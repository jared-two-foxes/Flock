
#include "Common/Model/Model.hpp"
#include "gtest/gtest.h"


TEST( Model, CreateEntity_ReturnsANewEntity )
{
  Model model;
  entity_t* entity = model.CreateEntity();

  EXPECT_NE( nullptr, entity );
}


TEST( Model, CreateEntity_CreatedEntityExistsInLocalList )
{
  Model model;
  entity_t* entity = model.CreateEntity();

  const std::vector<entity_t >& entities = model.Entities();
  auto it = std::find_if( entities.begin(), entities.end(), [&](const entity_t& e) { return ( e.identifier == entity->identifier ); });

  EXPECT_NE( entities.end(), it );
}

