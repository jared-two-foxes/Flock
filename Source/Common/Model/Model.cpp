
#include "Model.hpp"

#include <algorithm>


Model::Model()
{}


const entity_t*
Model::Get( int identifier ) const
{
  auto it = std::find_if( m_entities.begin(), m_entities.end(), [ & ]( const entity_t& e ) {
    return ( e.identifier == identifier );
  } );

  return ( it != m_entities.end() ) ? &( *it ) : nullptr;
}


entity_t*
Model::Get( int identifier )
{
  auto it = std::find_if( m_entities.begin(), m_entities.end(), [ & ]( entity_t& e ) {
    return ( e.identifier == identifier );
  } );

  return ( it != m_entities.end() ) ? &(*it) : nullptr;
}


entity_t*
Model::CreateEntity()
{
  static int identifier = 0;

  m_entities.push_back( entity_t() );
  entity_t* e = &m_entities.back();
  e->identifier = identifier++;
  return e;
}
