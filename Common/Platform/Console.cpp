
#include "Console.hpp"

#include <strsafe.h>

HANDLE g_handle;
CONSOLE_CURSOR_INFO g_Cursor;
  
void ErrorExit( LPTSTR lpszFunction ) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
        StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

void 
Console::Init()
{
 	g_handle = GetStdHandle( STD_OUTPUT_HANDLE );
  if( g_handle == INVALID_HANDLE_VALUE ) 
  {
    ErrorExit( "Console::Init" );
  }

  bool ret = GetConsoleCursorInfo( g_handle, &g_Cursor );
  if( !ret ) 
  {
    ErrorExit( "Console::Init" );
  }
}

void 
Console::Clear( char fill ) 
{ 
  COORD tl = {0,0};
	CONSOLE_SCREEN_BUFFER_INFO s;
	GetConsoleScreenBufferInfo( g_handle, &s );
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter( g_handle, fill, cells, tl, &written );
	FillConsoleOutputAttribute( g_handle, s.wAttributes, cells, tl, &written );
	SetConsoleCursorPosition( g_handle, tl );
}

void 
Console::SetCursorVisible( bool bVisible ) 
{
  g_Cursor.bVisible = bVisible;
  bool bRet = SetConsoleCursorInfo( g_handle, &g_Cursor );
  if( !bRet ) 
  {
    ErrorExit( "Console::SetCursorVisible" );
  }
}

void
Console::SetCursorPosition( COORD& coord ) 
{
  bool ret = SetConsoleCursorPosition( g_handle, coord );
  if( !ret ) 
  {
    ErrorExit( "Console::SetCursorPosition" );
  }
}

void
Console::GetScreenBufferInfo( CONSOLE_SCREEN_BUFFER_INFO* pScreenBufferInfo )
{
  bool ret = GetConsoleScreenBufferInfo( g_handle, pScreenBufferInfo );
  if( !ret ) 
  {
    ErrorExit( "Console::GetScreenBufferInfo" );
  }
}


