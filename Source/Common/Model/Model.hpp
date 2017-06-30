#ifndef _FLOCK_MODEL_HPP_
#define _FLOCK_MODEL_HPP_

#include <Common/Entity/Entity.hpp>

#include <vector>

class Model
{
public:
  std::vector<entity_t > m_entities;

public:
  Model();

  virtual ~Model() {}

  std::vector<entity_t >& Entities() { return m_entities; }
  const std::vector<entity_t >& Entities() const { return m_entities; }

  std::size_t Count() const { return m_entities.size(); }
  const entity_t* Get( int identifier ) const;
  entity_t* Get( int identifier );

  virtual entity_t* CreateEntity();

};

#endif // _FLOCK_MODEL_HPP_