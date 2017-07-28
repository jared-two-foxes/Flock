#ifndef FLOCK_COMMON_CONSOLE_H__
#define FLOCK_COMMON_CONSOLE_H__

#if defined(WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


struct console_t
///
///
///
{
  HANDLE handle;
  CONSOLE_CURSOR_INFO cursor;
  CONSOLE_SCREEN_BUFFER_INFO screen_info;
};

console_t Create();

void Clear( console_t& console, char fill = ' ' );

void SetCursorVisible( console_t& console, bool bVisible );

void SetCursorPosition( console_t& console, COORD& coord );
COORD GetCursorPosition( console_t& console );
  
#endif // FLOCK_COMMON_CONSOLE_H__