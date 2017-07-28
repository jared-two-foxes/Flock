
#include <Server/Server.hpp>


int main( int argc, char* argv[] )
{
  const std::string publisherEndPoint = "tcp://*:5556";
  const std::string connectListenerEndPoint = "tcp://*:5555";

  Model model;
  GameController controller( &model );
  Server server( &model, &controller );
  server.Init( publisherEndPoint.c_str(), connectListenerEndPoint.c_str() );

  float elapsed;
  std::chrono::high_resolution_clock::time_point last;
  last = std::chrono::high_resolution_clock::now();

  while ( 1 )
  {
    last = server.Update( last );
    server.PrintToConsole();
  }

  return 0;
}