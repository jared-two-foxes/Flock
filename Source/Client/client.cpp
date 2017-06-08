//
//  Weather update client in C++
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode
//

#include "ClientState.hpp"

#include <Nebulae/Common/Platform/Win32/Win32Platform.h>
#include <Nebulae/Beta/StateStack/StateStack.h>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nShowCmd )
{
  // Create the application.
  Nebulae::StateStack app;
  app.Initiate( 800, 600 );
  app.PushState( new ClientState() );
  app.Run();
  
  return 0;
}