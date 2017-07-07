
#include "ClientState.hpp"

#include <Nebulae/Beta/Camera/Camera.h>
#include <Nebulae/Beta/StateStack/StateStack.h>

#include <Nebulae/Alpha/Buffer/HardwareBuffer.h>
#include <Nebulae/Alpha/InputLayout/InputLayout.h>
#include <Nebulae/Alpha/InputLayout/VertexDeceleration.h>
#include <Nebulae/Alpha/RenderSystem/RenderSystem.h>

#include <Nebulae/Common/Window/Window.h>

#include <Brofiler.h>

#include <iostream>
#include <sstream>


using namespace Nebulae;

int identifier = -1;
volatile bool ready = false;

ClientState::KeyBinding bindings[][ 4 ] =
{
  {
    { Nebulae::VKC_LEFT, ClientState::INPUT_ACTION_LEFT },
    { Nebulae::VKC_RIGHT, ClientState::INPUT_ACTION_RIGHT },
    { Nebulae::VKC_UP, ClientState::INPUT_ACTION_UP },
    { Nebulae::VKC_DOWN, ClientState::INPUT_ACTION_DOWN }
  },
  {
    { Nebulae::VKC_A, ClientState::INPUT_ACTION_LEFT },
    { Nebulae::VKC_D, ClientState::INPUT_ACTION_RIGHT },
    { Nebulae::VKC_W, ClientState::INPUT_ACTION_UP },
    { Nebulae::VKC_S, ClientState::INPUT_ACTION_DOWN }
  }
};


std::size_t
DrawCircle( const vector2_t& p, float r, int num_segments, const vector4_t& d, std::vector<float >* vertices )
// GL_LINE_LOOP
{
  float theta = 2 * 3.1415926f / float( num_segments );
  float c = cosf( theta );//precalculate the sine and cosine
  float s = sinf( theta );
  float t;

  float x = r; //we start at angle = 0 
  float y = 0;

  std::size_t i = 0, ii = 0;

  // To create using a triangle fan.
  ( *vertices )[ i++ ] = p.x;
  ( *vertices )[ i++ ] = p.y;
  ( *vertices )[ i++ ] = d.x;
  ( *vertices )[ i++ ] = d.y;
  ( *vertices )[ i++ ] = d.z;
  ( *vertices )[ i++ ] = d.w;

  for ( ; ii < num_segments + 1; ii++ )
  {
    // Output vertex 
    ( *vertices )[ i++ ] = x + p.x;
    ( *vertices )[ i++ ] = y + p.y;

    // Output colour
    ( *vertices )[ i++ ] = d.x;
    ( *vertices )[ i++ ] = d.y;
    ( *vertices )[ i++ ] = d.z;
    ( *vertices )[ i++ ] = d.w;

    //apply the rotation matrix
    t = x;
    x = c * x - s * y;
    y = s * t + c * y;
  }

  return ( ii + 1 );
}


std::size_t
DrawArrow( const vector2_t& p, const vector2_t& dir, float r, const vector4_t& d, std::vector<float >* vertices )
// GL_TRIANGLES
{
  vector2_t offsets[] = { {0,r}, {0,-r}, {r,-r}, {0,r}, {-r,-r}, {0,-r} };

  static vector2_t up( 0.0f, 1.0f ), left( 1.0f, 0 );
  float perp_dot = Dot( left, dir );
  float dot = Dot( up, dir );
  float a = atan2( perp_dot, dot );
  float c = cosf( a );
  float s = sinf( a );
  matrix2_t rot ( c, -s, s, c );
  
  std::size_t i = 0, ii = 0;
  for ( ; ii < 6; ++ii )
  {
    vector2_t off = rot * offsets[ ii ];

    ( *vertices )[ i++ ] = p.x + off.x;
    ( *vertices )[ i++ ] = p.y + off.y;
    ( *vertices )[ i++ ] = d.x;
    ( *vertices )[ i++ ] = d.y;
    ( *vertices )[ i++ ] = d.z;
    ( *vertices )[ i++ ] = d.w;
  }

  return ii;
}


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
  m_lastDirection( 0.0f, 1.0f ),
  context( std::make_unique<zmq::context_t>( 1 ) ),
  localSocket( std::make_unique<zmq::socket_t>( *context, ZMQ_REQ ) ),
  serverSocket( std::make_unique<zmq::socket_t>( *context, ZMQ_REQ ) ),
  m_lag( 0 ),
  m_lastCommand( "" )
{
  AllocConsole();
  freopen( "CONOUT$", "w", stdout );
  freopen( "CONOUT$", "w", stderr );
}


ClientState::~ClientState()
{}


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
    snprintf( ( char * ) message.data(), 20, "new" );
    serverSocket->send( message );

    zmq::message_t reply;
    serverSocket->recv( &reply ); //< blocking

    //@todo check that the reply is an int for the identifier.
    std::istringstream iss( static_cast< char* >( reply.data() ) );
    iss >> identifier;
  }
  catch ( zmq::error_t e )
  {
    std::cout << e.what() << std::endl;
  }

  // Register ourselves as an inputlistener.
  SetBinding( 0 );
  caller->AddInputListener( this );
}


void
ClientState::Exit( Nebulae::StateStack* caller )
{}


void
ClientState::Update( float fDeltaTimeStep, Nebulae::StateStack* pCaller )
{
  BROFILER_CATEGORY( "ClientState::Update", Profiler::Color::AliceBlue );

  SendClientUpdate();

  if ( !TryServerUpdate() )
  {
    // The server update hasn't completed yet, simulate this frame.
    SimulateStep( fDeltaTimeStep );
  }

  // Check the direction and update if required.
  auto it = std::find_if( entities.begin(), entities.end(), [ & ]( entity_t& e ) { return ( e.identifier == identifier ); } );
  if ( it != entities.end() )
  {
    if ( LengthSq( it->direction ) > 0.001f )
    {
      m_lastDirection = it->direction;
    }
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
  for ( auto& e : entities )
  {
    std::size_t count = 0;

    // Reset the Entity buffer for this entity.
    if ( e.player )
    {
      vector4_t colour = ( e.identifier == identifier ) ? vector4_t( 0, 1.0f, 0, 1.0f ) : vector4_t( 0, 0, 1.0f, 1.0f );

      count = DrawArrow( e.position, m_lastDirection, e.radius, colour, &vertices );
    }
    else
    {
      vector4_t colour( 1.0f, 0.0f, 0.0f, 1.0f );
      count = DrawCircle( e.position, e.radius, 32, colour, &vertices );
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
ClientState::SetBinding( int variant )
{
  m_inputBindings.push_back( bindings[ variant ][ 0 ] );
  m_inputBindings.push_back( bindings[ variant ][ 1 ] );
  m_inputBindings.push_back( bindings[ variant ][ 2 ] );
  m_inputBindings.push_back( bindings[ variant ][ 3 ] );
}


void
ClientState::KeyPressed( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  // Iterate the keybindings to see if this keypress should do anything.
  auto it = std::find_if( m_inputBindings.begin(), m_inputBindings.end(), [ & ]( KeyBinding& binding ) {
    return ( binding.key == keyCode );
  } );

  if ( it != m_inputBindings.end() )
  {
    // Only add the action into the active actions list so long as its not already pressed.
    auto j = std::find( m_activeActions.begin(), m_activeActions.end(), it->action );
    if ( j == m_activeActions.end() )
    {
      m_activeActions.push_back( it->action );
    }
  }
}


void
ClientState::KeyReleased( Nebulae::KeyCode keyCode, uint32 key_code_point, Nebulae::Flags<Nebulae::ModKey> modKeys )
{
  if ( keyCode == Nebulae::VKC_1 )
  {
    SetBinding( 0 );
    m_activeActions.clear();
    m_lastCommand = "";
  }
  else if ( keyCode == Nebulae::VKC_2 )
  {
    SetBinding( 1 );
    m_activeActions.clear();
    m_lastCommand = "";
  }
  else
  {
    // Iterate the keybindings to see if this keypress maps to anything.
    auto it = std::find_if( m_inputBindings.begin(), m_inputBindings.end(), [ & ]( KeyBinding& binding ) {
      return ( binding.key == keyCode );
    } );

    // If found remove from the action queue.
    if ( it != m_inputBindings.end() )
    {
      m_activeActions.erase( std::remove( m_activeActions.begin(), m_activeActions.end(), it->action ) );
    }
  }
}


std::string
ClientState::GetCurrentCommand() const
{
  std::string command( "" );

  for ( auto action : m_activeActions )
  {
    if ( action == INPUT_ACTION_LEFT )
    {
      command += ",left";
    }
    else if ( action == INPUT_ACTION_RIGHT )
    {
      command += ",right";
    }
    else if ( action == INPUT_ACTION_UP )
    {
      command += ",up";
    }
    else if ( action == INPUT_ACTION_DOWN )
    {
      command += ",down";
    }
  }

  if ( command.size() >= 1 )
  {
    command.erase( 0, 1 ); //< pop off the front ',' character
  }

  return command;
}


vector2_t
ClientState::GetDirectionFromCommand( const std::string& cmd )
{
  vector2_t d( 0, 0 );

  if ( std::strstr( cmd.c_str(), "left" ) != nullptr )
  {
    d.x -= 1.0f;
  }
  if ( std::strstr( cmd.c_str(), "right" ) != nullptr )
  {
    d.x += 1.0f;
  }
  if ( std::strstr( cmd.c_str(), "up" ) != nullptr )
  {
    d.y += 1.0f;
  }
  if ( std::strstr( cmd.c_str(), "down" ) != nullptr )
  {
    d.y -= 1.0f;
  }

  if ( d.x > 0 || d.y > 0 )
  {
    return Normalize( d );
  }

  return d;
}

void
ClientState::SendClientUpdate()
///
/// Attempt to send the Server a update message containing any relevant player information 
/// which should result in the player changing behaviour in the server's simulation of the 
/// game world. 
///
{
  NE_ASSERT( pressedKey != Nebulae::VKC_UNKNOWN, "No valid keypress to send to the Server found" )( );

  static int state = 0;

  auto it = std::find_if( entities.begin(), entities.end(), []( entity_t& e ) { return e.identifier == identifier; } )
    NE_ASSERT( it != entities.end(), "Unable to find the player entity" )( identifier );

  try
  {
    if ( state == 0 )
    {
      std::string command = GetCurrentCommand();
      if ( command != m_lastCommand )
      {
        // Compose the message to send.
        zmq::message_t request( 32 );
        snprintf( ( char * ) request.data(), 32, "%05d%s", identifier, command.c_str() );

        // Attempt to send command to server.
        if ( serverSocket->send( request, ZMQ_NOBLOCK ) )
        {
          it->direction = GetDirectionFromCommand( command );
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
      entities.resize( list_size );
      assert( update.size() >= sizeof( long long ) + sizeof( int ) + list_size * sizeof( entity_t ) );
      memcpy( &entities[ 0 ], data_ptr + sizeof( long long ) + sizeof( int ), list_size * sizeof( entity_t ) ); //< Smash over the top all entities with info from server.

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
{
  // Attempt to simulate the movement based upon the last known directions
  for ( auto& e : entities )
  {
    e.position = e.position + e.direction * e.speed * fDeltaTimeStep;
  }
}