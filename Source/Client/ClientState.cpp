
#include "ClientState.hpp"
#include "ActionBinding.hpp"
#include "PlayerController.hpp"
#include "DrawingHelper.hpp"

#include <Nebulae/Beta/Camera/Camera.h>
#include <Nebulae/Beta/StateStack/StateStack.h>

#include <Nebulae/Alpha/Buffer/HardwareBuffer.h>
#include <Nebulae/Alpha/InputLayout/InputLayout.h>
#include <Nebulae/Alpha/InputLayout/VertexDeceleration.h>
#include <Nebulae/Alpha/RenderSystem/RenderSystem.h>

#include <Nebulae/Common/Window/Window.h>

#include <Brofiler.h>

#include <boost/tokenizer.hpp>

#include <iostream>
#include <sstream>


using namespace Nebulae;


KeyBinding bindings[][ 4 ] =
{
  {
    { Nebulae::VKC_LEFT, PlayerAction::LEFT },
    { Nebulae::VKC_RIGHT, PlayerAction::RIGHT },
    { Nebulae::VKC_UP, PlayerAction::UP },
    { Nebulae::VKC_DOWN, PlayerAction::DOWN }
  },
  {
    { Nebulae::VKC_A, PlayerAction::LEFT },
    { Nebulae::VKC_D, PlayerAction::RIGHT },
    { Nebulae::VKC_W, PlayerAction::UP },
    { Nebulae::VKC_S, PlayerAction::DOWN }
  }
};

volatile bool ready = false;




void Thread()
{
  zmq::context_t context( 1 );
  zmq::socket_t subscriber( context, ZMQ_SUB );
  zmq::socket_t reply( context, ZMQ_REP );

  try
  {
    reply.bind( "tcp://*:5558" );

    subscriber.connect( "tcp://localhost:5556" );

    // Setting subscription to all events. Argument is not currently supported, server doesnt push a identifier yet.
    const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
    subscriber.setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );

    // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
    int conflate = 1;
    subscriber.setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );
  }
  catch ( zmq::error_t& e )
  {
    std::cout << e.what();
  }

  zmq::message_t update;
  ready = true;

  while ( 1 )
  {
    try
    {
      // Check if there is a new server subscription message.
      zmq::message_t next;
      if ( subscriber.recv( &next, ZMQ_NOBLOCK ) )
      {
        // Only store this update if it has data.
        if ( next.size() > 0 )
        {
          update = std::move( next );
        }
      }
    }
    catch ( zmq::error_t& e )
    {
      std::cout << e.what();
    }

    try
    {
      // Check if we were asked for an update.
      zmq::message_t request;
      if ( reply.recv( &request, ZMQ_NOBLOCK ) )
      {
        reply.send( update );
      }
    }
    catch ( zmq::error_t& e )
    {
      std::cout << e.what();
    }
  }
}


ClientState::ClientState() :
  State( "Client" ),
  m_pCamera( NULL ),
  context( std::make_unique<zmq::context_t>( 1 ) ),
  localSocket( std::make_unique<zmq::socket_t>( *context, ZMQ_REQ ) ),
  serverSocket( std::make_unique<zmq::socket_t>( *context, ZMQ_REQ ) ),
  m_lag( 0 )
{
  //AllocConsole();
  //freopen( "CONOUT$", "w", stdout );
  //freopen( "CONOUT$", "w", stderr );
}


ClientState::~ClientState()
{
  for ( PlayerController* player : m_players )
  {
    delete player;
  }
  m_players.clear();
}


void
ClientState::Enter( Nebulae::StateStack* caller )
{
  m_fetch_thread = std::thread( Thread );
  while ( !ready ); //Wait until we are ready to go.

  // Grab Application variables to help with setup.
  m_pRenderSystem = caller->GetRenderSystem();

  // Grab the window object
  RenderSystem::WindowPtr window = m_pRenderSystem->GetWindow();
  float aspectRatio = float( window->GetWidth() ) / window->GetHeight();

  // Setup camera
  m_pCamera = std::make_shared<Camera >();
  Vector4 vCameraEye( 0.0f, 0.0f, 10.0f );
  Vector4 vLookAt( 0.0f, 0.0f, 0.0f );
  Vector4 vUp( 0.0f, 1.0f, 0.0f );
  m_pCamera->LookAt( vCameraEye, vLookAt, vUp );
  m_pCamera->SetOrtho( 300 * aspectRatio, 300, 0.1f, 20.0f );

  // Attempt to create a vertex shader.
  HardwareShader* pVertexShader = m_pRenderSystem->CreateShader(
    "../../Media/vs.glsl", VERTEX_SHADER );

  // Attempt to create a fragment shader.
  HardwareShader* pPixelShader = m_pRenderSystem->CreateShader(
    "../../Media/fs.glsl", PIXEL_SHADER );

  // @todo [jared.watt 25.05.2013] Leaks. Needs to be deleted somewhere.
  VertexDeceleration* pVertexDecl = new VertexDeceleration( 3 );
  pVertexDecl->AddElement( VET_FLOAT2, VES_POSITION, 0 );
  pVertexDecl->AddElement( VET_FLOAT4, VES_DIFFUSE, 0 );

  // Attempt to create an input layout
  InputLayout* pInputLayout = m_pRenderSystem->CreateInputLayout(
    "DebugLayout", pVertexDecl, pVertexShader );

  // Setup a buffer for the entities
  {
    std::vector<float > vertices( 64 * 6 );
    std::fill( vertices.begin(), vertices.end(), 1.0f );

    // Attempt to create a Buffer of video memory
    HardwareBuffer* pBuffer = m_pRenderSystem->CreateBuffer(
      "EntitiesBuffer",
      HBU_STATIC,
      64 * 6 * sizeof( Real ),
      HardwareBufferBinding::HBB_VERTEX,
      &vertices[ 0 ] );
  }

  // Setup a vertex buffer for the cube which is the play area
  {
    box_t zone;
    zone.a = vector4_t( -100.0f, -100.0f, -100.0f, 0 );
    zone.b = vector4_t( 100.0f, 100.0f, 100.0f, 0 );

    int i = 0;
    std::vector<float > vertices( 8 * 6 );
    std::fill( vertices.begin(), vertices.end(), 1.0f );
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; i += 6;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; i += 6;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; i += 6;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; i += 6;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; i += 6;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; i += 6;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; i += 6;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; i += 6;

    // Attempt to create a Buffer of video memory
    HardwareBuffer* pBuffer = m_pRenderSystem->CreateBuffer(
      "BoxBuffer",
      HBU_STATIC,
      8 * 6 * sizeof( Real ),
      HardwareBufferBinding::HBB_VERTEX,
      &vertices[ 0 ] );
  }


  // Setup the socket connections to the local socket for grabing server Updates.
  SetupServerSocket();

  // Register ourselves as an inputlistener.
  caller->AddInputListener( this );
}


void
ClientState::Exit( Nebulae::StateStack* caller )
{}


void
ClientState::Update( float fDeltaTimeStep, Nebulae::StateStack* pCaller )
{
  BROFILER_CATEGORY( "ClientState::Update", Profiler::Color::AliceBlue );

  TrySendClientUpdate();

  if ( !TryServerUpdate() )
  {
    // The server update hasn't completed yet, simulate this frame.
    SimulateStep( fDeltaTimeStep );
  }
}


void
ClientState::Render() const
{
  BROFILER_CATEGORY( "ClientState::Render", Profiler::Color::Violet );

  static long long last_server_time = std::numeric_limits<long long >::min();
  static std::chrono::high_resolution_clock::time_point
    _last = std::chrono::high_resolution_clock::now(),
    _last_frame_time = std::chrono::high_resolution_clock::now();
  static int _frame_count = 0;
  static float frame_delta = 0;
  static float fps = 0;

  auto now = std::chrono::high_resolution_clock::now();
  frame_delta = std::chrono::duration<float, std::milli >( now - _last_frame_time ).count();
  _last_frame_time = now;

  if ( _frame_count >= 100 )
  {
    std::chrono::duration<float > delta = now - _last;

    fps = _frame_count / delta.count();
    _frame_count = 0;
    _last = now;
  }
  else
  {
    _frame_count++;
  }

  {
    BROFILER_CATEGORY( "PrepareShader", Profiler::Color::CornflowerBlue );

    HardwareShader* pVertexShader = m_pRenderSystem->FindShaderByName( "../../Media/vs.glsl" );
    HardwareShader* pPixelShader = m_pRenderSystem->FindShaderByName( "../../Media/fs.glsl" );
    m_pRenderSystem->SetShaders( pVertexShader, pPixelShader );

    // Create projection variable from desc.
    UniformDefinition& worldVarDesc = m_pRenderSystem->GetUniformByName( "world" );
    UniformDefinition& viewVarDesc = m_pRenderSystem->GetUniformByName( "view" );
    UniformDefinition& projectionVarDesc = m_pRenderSystem->GetUniformByName( "projection" );

    // Calculate the local transform of the particle
    Matrix4 worldMatrix;
    worldMatrix.SetIdentity();

    float world[ 16 ], view[ 16 ], projection[ 16 ];
    worldMatrix.GetOpenGL( &world[ 0 ] );
    m_pCamera->GetViewMatrix().GetOpenGL( &view[ 0 ] );
    m_pCamera->GetProjectionMatrix().GetOpenGL( &projection[ 0 ] );

    // Set camera transforms for pass
    m_pRenderSystem->SetUniformBinding( worldVarDesc, ( void* ) &world[ 0 ] );
    m_pRenderSystem->SetUniformBinding( viewVarDesc, ( void* ) &view[ 0 ] );
    m_pRenderSystem->SetUniformBinding( projectionVarDesc, ( void* ) &projection[ 0 ] );
  }

  // Draw Box Lines.
  {
    BROFILER_CATEGORY( "DrawBox", Profiler::Color::LightGreen );

    m_pRenderSystem->SetOperationType( OT_LINES );

    // Set the Vertex Buffer
    size_t iOffset = 0;
    size_t iStride = 6 * sizeof( Real );
    HardwareBuffer* pBuffer = m_pRenderSystem->FindBufferByName( "BoxBuffer" );
    m_pRenderSystem->SetVertexBuffers( 0, pBuffer, iStride, iOffset );

    // Set the Vertex input layout
    InputLayout* pInputLayout = m_pRenderSystem->FindInputLayoutByName( "DebugLayout" );
    m_pRenderSystem->SetInputLayout( pInputLayout );

    m_pRenderSystem->Draw( 8, 0 );
  }


  // Draw Entities.
  std::vector<float > vertices( 64 * 6 );
  for ( auto& e : m_entities )
  {
    std::size_t count = 0;

    // Reset the Entity buffer for this entity.
    if ( e.player )
    {
      auto it = std::find_if( m_players.begin(), m_players.end(), [ & ]( PlayerController* controller ) { return ( controller->Identifier() == e.identifier ); } );
      NE_ASSERT( it != m_players.end(), "Unable to find controller for player entities" )( );

      vector4_t colour( 0, 1.0f, 0, 1.0f );
      count = DrawingHelper::DrawArrow( e.position, e.direction, e.radius, colour, &vertices );
    }
    else
    {
      vector4_t colour( 1.0f, 0.0f, 0.0f, 1.0f );
      count = DrawingHelper::DrawCircle( e.position, e.radius, 32, colour, &vertices );
    }

    HardwareBuffer* pBuffer = m_pRenderSystem->FindBufferByName( "EntitiesBuffer" );
    if ( pBuffer )
    {
      std::size_t offset = 0;
      std::size_t length = count * 6 * sizeof( float );
      pBuffer->WriteData( offset, length, &vertices[ 0 ], true );
    }

    // Set the operation type
    m_pRenderSystem->SetOperationType( e.player ? OT_TRIANGLES : OT_TRIANGLEFAN );

    // Set the Vertex Buffer
    size_t iOffset = 0;
    size_t iStride = 6 * sizeof( Real );
    m_pRenderSystem->SetVertexBuffers( 0, pBuffer, iStride, iOffset );

    // Set the Vertex input layout
    InputLayout* pInputLayout = m_pRenderSystem->FindInputLayoutByName( "DebugLayout" );
    m_pRenderSystem->SetInputLayout( pInputLayout );

    // Draw Entity Dot.
    m_pRenderSystem->Draw( count, 0 );
  }
}


void
ClientState::KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  for ( PlayerController* controller : m_players )
  {
    controller->KeyPressed( keyCode, key_code_point, modKeys );
  }
}


void
ClientState::KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  if ( m_players.empty() && keyCode > Nebulae::KeyCode::VKC_0 && keyCode <= Nebulae::KeyCode::VKC_9 )
  {
    int generatePlayers = keyCode - Nebulae::KeyCode::VKC_0;
    TryCreatePlayers( generatePlayers );
  }

  for ( PlayerController* controller : m_players )
  {
    controller->KeyReleased( keyCode, key_code_point, modKeys );
  }
}


void
ClientState::SetupServerSocket()
{
  try
  {
    localSocket->connect( "tcp://localhost:5558" );
  }
  catch ( zmq::error_t e )
  {
    std::cout << e.what() << std::endl;
  }

  // Setup the socket connection to the server for user input.
  try
  {
    serverSocket->connect( "tcp://localhost:5555" );

    // Let the server know that we have connected!
    zmq::message_t message( 20 );
    snprintf( ( char * ) message.data(), 20, "join" );
    serverSocket->send( message );

    zmq::message_t reply;
    serverSocket->recv( &reply ); //< blocking

                                  //@todo Check that the reply is an int for the client identifier.
    std::istringstream iss( static_cast< char* >( reply.data() ) );
    iss >> m_clientId;
  }
  catch ( zmq::error_t e )
  {
    std::cout << e.what() << std::endl;
  }
}


std::string
ClientState::CreateClientMessage()
{
  std::string msg;
  msg += std::to_string( m_clientId );

  // Build the client update message.
  for ( PlayerController* controller : m_players )
  {
    const std::vector<PlayerAction >& actions = controller->ActiveActions();

    // Create the client->server message for this controller.
    std::string command;
    for ( auto action : actions )
    {
      if ( action == PlayerAction::LEFT )
      {
        command += ",left";
      }
      else if ( action == PlayerAction::RIGHT )
      {
        command += ",right";
      }
      else if ( action == PlayerAction::UP )
      {
        command += ",up";
      }
      else if ( action == PlayerAction::DOWN )
      {
        command += ",down";
      }
    }

    if ( command.size() >= 1 )
    {
      command.erase( 0, 1 ); //< pop off the front ',' character
    }

    msg += " ";
    msg += std::to_string( controller->Identifier() );
    msg += " ";
    msg += command;
  }

  return msg;
}


void
ClientState::ProcessClientMessage( const std::string& msg )
{
  boost::char_separator<char> sep( " " );
  boost::tokenizer<boost::char_separator<char> > tokens( msg, sep );

  std::vector<std::string > list;
  for ( auto it = tokens.begin(); it != tokens.end(); ++it )
  {
    list.push_back( *it );
  }

  // First token is going to be our own identifier
  NE_ASSERT( atoi( list[ 0 ].c_str() ) == m_clientId, "Client message appears to be in an incorrect format" )( msg );

  int i = 1;
  for ( ; i < list.size(); ++i )
  {
    // Find the entity.
    int id = atoi( list[ i++ ].c_str() );

    auto it = std::find_if( m_entities.begin(), m_entities.end(), [ & ]( entity_t& e ) { return ( e.identifier == id ); } );
    if ( it != m_entities.end() )
    {
      vector2_t d( 0, 0 );
      if( i < list.size() )
      {
        if ( strstr( list[ i ].c_str(), "left" ) != nullptr )
        {
          d.x -= 1.0f;
        }
        if ( strstr( list[ i ].c_str(), "right" ) != nullptr )
        {
          d.x += 1.0f;
        }
        if ( strstr( list[ i ].c_str(), "up" ) != nullptr )
        {
          d.y += 1.0f;;
        }
        if ( strstr( list[ i ].c_str(), "down" ) != nullptr )
        {
          d.y -= 1.0f;
        }
      }

      if ( d.x > 0 || d.y > 0 )
      {
        d = Normalize( d );
      }

      it->direction = d;
    }
  }
}


void
ClientState::TryCreatePlayers( int count )
///
/// @todo Make non-blocking?
///
{
  // Setup the socket connection to the server for user input.
  try
  {
    // Let the server know that we have connected!
    zmq::message_t message( 20 );
    snprintf( ( char * ) message.data(), 20, "create %d", count );
    serverSocket->send( message );

    zmq::message_t reply;
    if ( serverSocket->recv( &reply ) ) //< blocking
    {
      // Check that the reply is a success code and extract the player identifiers.
      std::string str( static_cast< char* >( reply.data() ) );
      boost::char_separator<char> sep( " " );
      boost::tokenizer<boost::char_separator<char> > tokens( str, sep );

      std::vector<std::string > tokenList;
      for ( auto it = tokens.begin(); it != tokens.end(); ++it )
      {
        tokenList.push_back( *it );
      }

      const int OK = 0;
      if ( tokenList[ 0 ] != "OK" )
      {
        //@todo handle the failure?
      }
      else
      {
        for ( int i = 1; i < tokenList.size(); ++i )
        {
          PlayerController* controller = new PlayerController( atoi( tokenList[ i ].c_str() ) );

          std::size_t variant = m_players.size();
          controller->SetBinding( bindings[ variant ][ 0 ].action, bindings[ variant ][ 0 ].key );
          controller->SetBinding( bindings[ variant ][ 1 ].action, bindings[ variant ][ 1 ].key );
          controller->SetBinding( bindings[ variant ][ 2 ].action, bindings[ variant ][ 2 ].key );
          controller->SetBinding( bindings[ variant ][ 3 ].action, bindings[ variant ][ 3 ].key );

          m_players.push_back( controller );
        }
      }
    }
  }
  catch ( zmq::error_t e )
  {
    std::cout << e.what() << std::endl;
  }
}


void
ClientState::TrySendClientUpdate()
///
/// Attempt to send the Server a update message containing any relevant player information 
/// which should result in the player changing behaviour in the server's simulation of the 
/// game world. 
///
{
  NE_ASSERT( pressedKey != Nebulae::VKC_UNKNOWN, "No valid keypress to send to the Server found" )( );

  static int state = 0;

  try
  {
    if ( state == 0 )
    {
      std::string command = CreateClientMessage();
      if ( command != m_lastCommand )
      {
        NE_ASSERT( command.length() < 128, "Client message is larger than Request buffer." )( );

        // Compose the message to ssend.
        zmq::message_t request( 128 );
        memcpy( request.data(), command.c_str(), command.length() );

        // Attempt to send command to server.
        if ( serverSocket->send( request, ZMQ_NOBLOCK ) )
        {
          ProcessClientMessage( command );
          m_lastCommand = command;
          state++;
        }
      }
    }

    if ( state == 1 )
    {
      // Wait on the receipt back from the Server that it received the update.
      zmq::message_t reply;
      if ( serverSocket->recv( &reply, ZMQ_NOBLOCK ) )
      {
        // @todo - should probably only set the direction at this point.  
        // @todo - should send message with a timestamp and account for time difference for motion on the server?
        state = 0;
      }
    }
  }
  catch ( zmq::error_t& e )
  {
    NE_LOG( e.what() );
  }
  catch ( ... )
  {
    NE_LOG( "Unhandled exception!" );
  }
}


bool
ClientState::TryServerUpdate()
///
/// Attempts to poll the server "heartbeat" listener socket for the latest server update.  
///
{
  static long long last_server_time = std::numeric_limits<long long >::min();
  static int       state = 0;

  try
  {
    zmq::message_t update;

    // Request an update from the local listener socket.
    if ( state == 0 )
    {
      zmq::message_t request( 1 );
      if ( localSocket->send( request, ZMQ_NOBLOCK ) )
      {
        state++;
      }
    }

    // Receive the Server update.
    if ( state == 1 )
    {
      if ( localSocket->recv( &update, ZMQ_NOBLOCK ) )
      {
        if ( update.size() > 0 )
        {
          state++;
        }
        else
        {
          state = 0; //< update received didnt contain any information, disregard and start again.
        }
      }
    }

    // Process retrieved server data.
    if ( state == 2 )
    {
      int list_size;
      long long timestamp;
      char* data_ptr = ( char* ) update.data();
      assert( update.size() >= sizeof( long long ) );
      memcpy( &timestamp, data_ptr, sizeof( long long ) );
      assert( update.size() >= sizeof( long long ) + sizeof( int ) );
      memcpy( &list_size, data_ptr + sizeof( long long ), sizeof( int ) );
      m_entities.resize( list_size );
      assert( update.size() >= sizeof( long long ) + sizeof( int ) + list_size * sizeof( entity_t ) );
      memcpy( &m_entities[ 0 ], data_ptr + sizeof( long long ) + sizeof( int ), list_size * sizeof( entity_t ) ); //< Smash over the top all entities with info from server.

      assert( last_server_time < timestamp );
      last_server_time = timestamp;

      static std::chrono::high_resolution_clock::time_point epoch;
      auto since_epoch = std::chrono::nanoseconds( last_server_time );
      auto server_time = epoch + since_epoch;

      auto now = std::chrono::high_resolution_clock::now();
      auto now_since_epoch = now.time_since_epoch();
      assert( server_time < now );

      m_lag = std::chrono::duration_cast< std::chrono::milliseconds >( now - server_time ).count();

      // Reset the state so that we start the process over.
      state = 0;

      return true;
    }
  }
  catch ( zmq::error_t e )
  {
    NE_LOG( e.what() );
  }
  catch ( ... )
  {
    NE_LOG( "Unhandled exception!" );
  }

  return false;
}


void
ClientState::SimulateStep( float fDeltaTimeStep )
///
/// Attempt to simulate the movement based upon the last known directions
///
{
  for ( auto& e : m_entities )
  {
    e.position = e.position + e.direction * e.speed * fDeltaTimeStep;
  }
}