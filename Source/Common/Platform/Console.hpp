#ifndef CONSOLE_H__
#define CONSOLE_H__

#if defined(WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Console
{

	void Init();

	void Clear( char fill = ' ' );

  void SetCursorVisible( bool bVisible );	

  void SetCursorPosition( COORD& coord );

  void GetScreenBufferInfo( CONSOLE_SCREEN_BUFFER_INFO* pScreenBufferInfo );
  
}

#endif // CONSOLE_H__