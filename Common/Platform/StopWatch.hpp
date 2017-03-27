#ifndef STOPWATCH_H__
#define STOPWATCH_H__

#if defined(WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
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