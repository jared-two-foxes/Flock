
#include "Console.hpp"

#include <strsafe.h>

  
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

console_t
Create()
{
  HANDLE handle_ = GetStdHandle( STD_OUTPUT_HANDLE );
  if( handle_ == INVALID_HANDLE_VALUE )
  {
    AllocConsole();
    handle_ = GetStdHandle( STD_OUTPUT_HANDLE );
    if( handle_ == INVALID_HANDLE_VALUE )
    {
      //ErrorExit( L"Console::Init" );
    }
  }

  bool ret;
  CONSOLE_CURSOR_INFO cursor_;
  ret = GetConsoleCursorInfo( handle_, &cursor_ );
  if( !ret ) 
  {
    ErrorExit( L"Console::Init" );
  }

  CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo_;
  ret = GetConsoleScreenBufferInfo( handle_, &screenBufferInfo_ );
  if ( !ret )
  {
    ErrorExit( L"Console::GetScreenBufferInfo" );
  }

  console_t console;
  console.handle = handle_;
  console.cursor = cursor_;
  console.screen_info = screenBufferInfo_;

  return console;
}

void 
Clear( console_t& console, char fill )
{ 
  bool ret;
  
  COORD tl = {0,0};
	DWORD written, cells = console.screen_info.dwSize.X * console.screen_info.dwSize.Y;
	
  ret = FillConsoleOutputCharacter( console.handle, fill, cells, tl, &written );
  if ( !ret )
  {
    ErrorExit( L"FillConsoleOutputCharacter" );
  }

	ret = FillConsoleOutputAttribute( console.handle, console.screen_info.wAttributes, cells, tl, &written );
  if ( !ret )
  {
    ErrorExit( L"FillConsoleOutputAttribute" );
  }

	ret = SetConsoleCursorPosition( console.handle, tl );
  if ( !ret )
  {
    ErrorExit( L"SetConsoleCursorPosition" );
  }
}

void 
SetCursorVisible( console_t& console, bool bVisible )
{
  console.cursor.bVisible = bVisible;
  bool bRet = SetConsoleCursorInfo( console.handle, &console.cursor );
  if( !bRet ) 
  {
    ErrorExit( L"Console::SetCursorVisible" );
  }
}

void
SetCursorPosition( console_t& console, COORD& coord )
{
  bool ret = SetConsoleCursorPosition( console.handle, coord );
  if( !ret ) 
  {
    ErrorExit( L"Console::SetCursorPosition" );
  }
}

COORD 
GetCursorPosition( console_t& console )
{
  bool ret; 

  // Update the screen info.
  ret = GetConsoleScreenBufferInfo( console.handle, &console.screen_info );
  if ( !ret )
  {
    ErrorExit( L"Console::GetScreenBufferInfo" );
  }

  // Return the current cursor position.
  return console.screen_info.dwCursorPosition;
}