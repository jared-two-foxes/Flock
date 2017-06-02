#ifndef STOPWATCH_H__
#define STOPWATCH_H__

#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#endif

class StopWatch 
{
private:
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_end;

  public:
    StopWatch();

    void Start();
    float Stop();
    float Read();
    

};

#endif // STOPWATCH_H__