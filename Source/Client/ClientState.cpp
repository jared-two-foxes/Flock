
#include "ClientState.hpp"
#include "ClientInputListener.hpp"

#include <Nebulae/Beta/Camera/Camera.h>
#include <Nebulae/Beta/StateStack/StateStack.h>

#include <Nebulae/Alpha/Buffer/HardwareBuffer.h>
#include <Nebulae/Alpha/InputLayout/InputLayout.h>
#include <Nebulae/Alpha/InputLayout/VertexDeceleration.h>
#include <Nebulae/Alpha/RenderSystem/RenderSystem.h>

#include <Nebulae/Common/Window/Window.h>

#include <iostream>
#include <sstream>


using namespace Nebulae;

int identifier = -1;


ClientState::ClientState() :
  State( "Client" ),
  m_pCamera( NULL ),
  m_pInputListener( std::make_unique<ClientInputListener >() ),
  context( std::make_unique<zmq::context_t>( 1 ) ),
  subscriber( std::make_unique<zmq::socket_t>( *context, ZMQ_SUB ) ),
  requestSocket( std::make_unique<zmq::socket_t>( *context, ZMQ_REQ ) ),
  pressedKey( Nebulae::VKC_UNKNOWN )
{}


ClientState::~ClientState()
{}


void
ClientState::Enter( Nebulae::StateStack* caller )
{
  // Grab Application variables to help with setup.
  m_pRenderSystem = caller->GetRenderSystem();

  // Grab the window object
  RenderSystem::WindowPtr window = m_pRenderSystem->GetWindow();
  float aspectRatio = float( window->GetWidth() ) / window->GetHeight();

  // Setup camera
  m_pCamera = std::make_shared<Camera >();
  Vector4 vCameraEye( 0.0f, 0.0f, -10.0f );
  Vector4 vLookAt( 0.0f, 0.0f, 0.0f );
  Vector4 vUp( 0.0f, 1.0f, 0.0f );
  m_pCamera->LookAt( vCameraEye, vLookAt, vUp );
  m_pCamera->SetOrtho( 300 * aspectRatio, 300, 0.1f, 20.0f );

  // Attempt to create a vertex shader.
  HardwareShader* pVertexShader = m_pRenderSystem->CreateShader(
    "vs.glsl", VERTEX_SHADER );

  // Attempt to create a fragment shader.
  HardwareShader* pPixelShader = m_pRenderSystem->CreateShader(
    "fs.glsl", PIXEL_SHADER );

  // @todo [jared.watt 25.05.2013] Leaks. Needs to be deleted somewhere.
  VertexDeceleration* pVertexDecl = new VertexDeceleration( 3 );
  pVertexDecl->AddElement( VET_FLOAT2, VES_POSITION, 0 );
  pVertexDecl->AddElement( VET_FLOAT4, VES_DIFFUSE, 0 );

  // Attempt to create an input layout
  InputLayout* pInputLayout = m_pRenderSystem->CreateInputLayout(
    "DebugLayout", pVertexDecl, pVertexShader );

  // Setup a buffer for the entities
  {
    std::vector<float > vertices( 256 * 7 );
    std::fill( vertices.begin(), vertices.end(), 1.0f );

    // Attempt to create a Buffer of video memory
    HardwareBuffer* pBuffer = m_pRenderSystem->CreateBuffer(
      "EntitiesBuffer",
      HBU_STATIC,
      256 * 6 * sizeof( Real ),
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


 // Setup the socket connections to the server
  {
    requestSocket->connect( "tcp://localhost:5555" );
    subscriber->connect( "tcp://localhost:5556" );

    // Setting subscription to all events. Argument is not currently supported, server doesnt push a identifier yet.
    const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
    subscriber->setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );

    // Let the server know that we have connected!
    zmq::message_t message( 20 );
    snprintf( ( char * ) message.data(), 20, "new" );
    requestSocket->send( message );

    zmq::message_t reply;
    requestSocket->recv( &reply ); //< blocking

    //@todo check that the reply is an int for the identifier.
    std::istringstream iss( static_cast< char* >( reply.data() ) );
    iss >> identifier;
  }

  // Add in the input listener.
  m_pInputListener->state = this;
  caller->AddInputListener( m_pInputListener.get() );
}


void
ClientState::Exit( Nebulae::StateStack* caller )
{}


void
ClientState::Update( float fDeltaTimeStep, Nebulae::StateStack* pCaller )
{
  // Attempt to pull a new update from publisher
  zmq::message_t update;

  if ( pressedKey != Nebulae::VKC_UNKNOWN )
  {
    zmq::message_t request( 32 );
    switch ( pressedKey )
    {
    case Nebulae::VKC_LEFT:
      snprintf( ( char * ) request.data(), 32, "%05d left", identifier );
      break;

    case Nebulae::VKC_RIGHT:
      snprintf( ( char * ) request.data(), 32, "%05d right", identifier );
      break;

    case Nebulae::VKC_UP:
      snprintf( ( char * ) request.data(), 32, "%05d up", identifier );
      break;

    case Nebulae::VKC_DOWN:
      snprintf( ( char * ) request.data(), 32, "%05d down", identifier );
      break;
    }

    requestSocket->send( request );

    // then we wait for a reply?  Don't know if I really care about getting confirmation?
    zmq::message_t reply;
    requestSocket->recv( &reply ); //?? - , ZMQ_NOBLOCK 
  }

  if ( subscriber->recv( &update, ZMQ_NOBLOCK ) )
  {
    // Process retrieved data.
    int list_size;
    char* data_ptr = ( char* ) update.data();
    memcpy( &list_size, data_ptr, sizeof( int ) );
    entities.resize( list_size );
    memcpy( &entities[ 0 ], data_ptr + sizeof( int ), list_size * sizeof( entity_t ) ); //< Smash over the top all entities with info from server.
    //ProcessUpdate( pushed_entities );

    // Update the render buffer with the current positions of the entities
    if ( entities.size() > 0 )
    {
      std::vector<float > vertices( entities.size() * 6 );
      std::fill( vertices.begin(), vertices.end(), 1.0f );

      int i = 0;
      for ( entity_t& e : entities )
      {
        vertices[ i + 0 ] = e.position.x;
        vertices[ i + 1 ] = e.position.y;

        vertices[ i + 2 ] = e.colour.x;
        vertices[ i + 3 ] = e.colour.y;
        vertices[ i + 4 ] = e.colour.z;
        vertices[ i + 5 ] = e.colour.w;

        i += 6;
      }

      std::size_t offset = 0;
      std::size_t length = entities.size() * 6 * sizeof( float );
      HardwareBuffer* pBuffer = m_pRenderSystem->FindBufferByName( "EntitiesBuffer" );
      //void* buf = pBuffer->Lock( Nebulae::HBL_DISCARD );
      pBuffer->WriteData( offset, length, &vertices[ 0 ], true );
      //pBuffer->Unlock();
    }
  }
}


void
ClientState::Render() const
{
  HardwareShader* pVertexShader = m_pRenderSystem->FindShaderByName( "vs.glsl" );
  HardwareShader* pPixelShader = m_pRenderSystem->FindShaderByName( "fs.glsl" );
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


  // Draw Box Lines.
  {
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
  {
    // Set the operation type
    m_pRenderSystem->SetOperationType( OT_POINTS );

    // Set the Vertex Buffer
    size_t iOffset = 0;
    size_t iStride = 6 * sizeof( Real );
    HardwareBuffer* pBuffer = m_pRenderSystem->FindBufferByName( "EntitiesBuffer" );
    m_pRenderSystem->SetVertexBuffers( 0, pBuffer, iStride, iOffset );

    // Set the Vertex input layout
    InputLayout* pInputLayout = m_pRenderSystem->FindInputLayoutByName( "DebugLayout" );
    m_pRenderSystem->SetInputLayout( pInputLayout );

    // Draw Entity Dot.
    m_pRenderSystem->Draw( entities.size(), 0 );
  }

}

void
ClientState::OnKeyUp( Nebulae::KeyCode keyCode )
{
  pressedKey = keyCode;
}