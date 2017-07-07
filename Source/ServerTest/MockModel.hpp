#ifndef __FLOCK_TEST_MOCKMODEL_H__
#define __FLOCK_TEST_MOCKMODEL_H__

#include <Common/Model/Model.hpp>

#include "gmock/gmock.h"


class MockModel : public Model
{
public:
  MOCK_METHOD0( CreateEntity, entity_t* () );
  
};


#endif // __FLOCK_TEST_MOCKMODEL_H__