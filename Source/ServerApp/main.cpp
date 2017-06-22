
#include <Server/Server.hpp>


int main( int argc, char* argv[] )
{
  const std::string publisherEndPoint = "tcp://*:5556";
  const std::string connectListenerEndPoint = "tcp://*:5555";

  Model model;
  GameController controller( &model );
  Server server( &model, &controller );
  server.Init( publisherEndPoint.c_str(), connectListenerEndPoint.c_str() );

  while ( 1 )
  {
    server.Update();
    server.PrintToConsole();
  }

  return 0;
}