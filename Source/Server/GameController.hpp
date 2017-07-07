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
  std::vector<int > m_players;

public:
  GameController( Model* m_model );

  std::vector<int >& GetPlayers() { return m_players; }
 
  virtual entity_t* AddPlayer();
  virtual void PrepareEntity( entity_t& e, const rect_t& zone );
  virtual void Update( float epsilon );

};

#endif // _FLOCK_MODEL_HPP_