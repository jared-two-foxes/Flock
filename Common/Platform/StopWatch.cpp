
#include "StopWatch.hpp"

#include <limits>

bool  init = false;
float ticks_per_second = std::numeric_limits<float >::max();

StopWatch::StopWatch()
{
  if( !init ) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency( &frequency);
    ticks_per_second = 1.0f / frequency.QuadPart;
    init = true;
  }
}

void 
StopWatch::Start() 
{ 
  QueryPerformanceCounter( &m_start ); 
}

float 
StopWatch::Stop() 
{ 
  QueryPerformanceCounter( &m_end);
	return Read();
}

float 
StopWatch::Read() 
{
  return ( ( m_end.QuadPart - m_start.QuadPart ) * ticks_per_second );
}
