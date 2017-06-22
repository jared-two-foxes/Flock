#ifndef _FLOCK_GAME_HPP_
#define _FLOCK_GAME_HPP_

#include <Common/Model/Model.hpp>

#include <chrono>

class GameController
{
public:
  static const std::chrono::duration<float > SPAWN_INTERVAL;

public:
  Model* m_model;
  rect_t m_zone;
  std::chrono::duration<float > m_secondsToNextSpawn;
  std::vector<entity_t* > m_players;

public:
  GameController( Model* m_model );

  virtual entity_t* AddPlayer();
  virtual void PrepareEntity( entity_t& e, const vector4_t& colour, const rect_t& zone );
  virtual void Update( float epsilon );

};

#endif // _FLOCK_MODEL_HPP_