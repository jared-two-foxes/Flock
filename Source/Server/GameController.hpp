#ifndef _FLOCK_GAME_HPP_
#define _FLOCK_GAME_HPP_

#include <Common/Model/Model.hpp>

#include <chrono>

struct Job;

class GameController
{
public:
  static const std::chrono::duration<float > SPAWN_INTERVAL;

public:
  Model* m_model;
  rect_t m_zone;
  std::vector<int > m_players;

  std::vector<Job* > m_jobs;

public:
  GameController( Model* m_model );

  std::vector<int >& GetPlayers() { return m_players; }
 
  void Init();
  virtual entity_t* AddPlayer();
  virtual void Update( float epsilon );

};

#endif // _FLOCK_MODEL_HPP_