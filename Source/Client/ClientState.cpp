
#include "ClientState.hpp"

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


ClientState::ClientState() :
  State( "Client" ),
  m_pCamera( NULL ),
  context( std::make_unique<zmq::context_t>( 1 ) ),
  subscriber( std::make_unique<zmq::socket_t>( *context, ZMQ_SUB ) )
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

  // Setup camera
  m_pCamera = std::make_shared<Camera >();
  Vector4 vCameraEye( -50.0f, 100.0f, 110.0f );
  Vector4 vLookAt( 0.0f, 0.0f, 0.0f );
  Vector4 vUp( 0.0f, 1.0f, 0.0f );
  m_pCamera->LookAt( vCameraEye, vLookAt, vUp );
  m_pCamera->SetOrtho( window->GetWidth(), window->GetHeight(), 0.1f, 1000.0f );

  // Attempt to create a vertex shader.
  HardwareShader* pVertexShader = m_pRenderSystem->CreateShader(
    "debug_vs.glsl", VERTEX_SHADER );

  // Attempt to create a fragment shader.
  HardwareShader* pPixelShader = m_pRenderSystem->CreateShader(
    "debug_fs.glsl", PIXEL_SHADER );

  // @todo [jared.watt 25.05.2013] Leaks. Needs to be deleted somewhere.
  VertexDeceleration* pVertexDecl = new VertexDeceleration( 3 );
  pVertexDecl->AddElement( VET_FLOAT3, VES_POSITION, 0 );
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
      256 * 7 * sizeof( Real ),
      HardwareBufferBinding::HBB_VERTEX,
      &vertices[ 0 ] );
  }

  // Setup a vertex buffer for the cube which is the play area
  {
    box_t zone;
    zone.a = vector4_t( -100.0f, -100.0f, -100.0f, 0 );
    zone.b = vector4_t( 100.0f, 100.0f, 100.0f, 0 );

    int i = 0;
    std::vector<float > vertices( 24 * 7 );
    std::fill( vertices.begin(), vertices.end(), 0.75f );
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.a.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.a.y; vertices[ i + 2 ] = zone.b.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.a.z; i += 7;
    vertices[ i + 0 ] = zone.b.x; vertices[ i + 1 ] = zone.b.y; vertices[ i + 2 ] = zone.b.z; i += 7;

    // Attempt to create a Buffer of video memory
    HardwareBuffer* pBuffer = m_pRenderSystem->CreateBuffer(
      "BoxBuffer",
      HBU_STATIC,
      24 * 7 * sizeof( Real ),
      HardwareBufferBinding::HBB_VERTEX,
      &vertices[ 0 ] );
  }


  // Setup the socket connection to the server
  {
    subscriber->connect( "tcp://localhost:5556" );

    // Setting subscription to all events. Argument is not currently supported, server doesnt push a identifier yet.
    const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
    subscriber->setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );
  }
}


void
ClientState::Exit( Nebulae::StateStack* caller )
{}


void
ClientState::Update( float fDeltaTimeStep, Nebulae::StateStack* pCaller )
{
  // Attempt to pull a new update from publisher
  zmq::message_t update;

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
      std::vector<float > vertices( 256 * 7 );
      std::fill( vertices.begin(), vertices.end(), 1.0f );

      int i = 0;
      for ( entity_t& e : entities )
      {
        vertices[ i + 0 ] = e.position.x;
        vertices[ i + 1 ] = e.position.y;
        vertices[ i + 2 ] = e.position.z;

        vertices[ i + 3 ] = 0.0f;
        vertices[ i + 4 ] = 0.0f;
        vertices[ i + 5 ] = 1.0f;
        vertices[ i + 6 ] = 1.0f;

        i += 7;
      }

      std::size_t offset = 0;
      std::size_t length = entities.size() * 7 * sizeof( float );
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
  HardwareShader* pVertexShader = m_pRenderSystem->FindShaderByName( "debug_vs.glsl" );
  HardwareShader* pPixelShader = m_pRenderSystem->FindShaderByName( "debug_fs.glsl" );
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
  //{
  //  m_pRenderSystem->SetOperationType( OT_LINES );

  //  // Set the Vertex Buffer
  //  size_t iOffset = 0;
  //  size_t iStride = 7 * sizeof( Real );
  //  HardwareBuffer* pBuffer = m_pRenderSystem->FindBufferByName( "BoxBuffer" );
  //  m_pRenderSystem->SetVertexBuffers( 0, pBuffer, iStride, iOffset );

  //  // Set the Vertex input layout
  //  InputLayout* pInputLayout = m_pRenderSystem->FindInputLayoutByName( "DebugLayout" );
  //  m_pRenderSystem->SetInputLayout( pInputLayout );

  //  m_pRenderSystem->Draw( 24, 0 );
  //}


  // Draw Entities.
  {
    // Set the operation type
    m_pRenderSystem->SetOperationType( OT_POINTS );

    // Set the Vertex Buffer
    size_t iOffset = 0;
    size_t iStride = 7 * sizeof( Real );
    HardwareBuffer* pBuffer = m_pRenderSystem->FindBufferByName( "EntitiesBuffer" );
    m_pRenderSystem->SetVertexBuffers( 0, pBuffer, iStride, iOffset );

    // Set the Vertex input layout
    InputLayout* pInputLayout = m_pRenderSystem->FindInputLayoutByName( "DebugLayout" );
    m_pRenderSystem->SetInputLayout( pInputLayout );

    // Draw Entity Dot.
    m_pRenderSystem->Draw( entities.size(), 0 );
  }
  
}