// Win32TwinTowers.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32TwinTowers.h"

using namespace alchemy;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AllocConsole();

	long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);  
	int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);  
	FILE* fp = _fdopen( hConHandle, "w" ); 
	*stdout = *fp;  
	setvbuf( stdout, NULL, _IONBF, 0 );  // 将 STDIN重定向到console  
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);  
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);  
	fp = _fdopen( hConHandle, "r" );  
	*stdin = *fp;  setvbuf( stdin, NULL, _IONBF, 0 );  //将 STDERR重定向到console  
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);  
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);  
	fp = _fdopen( hConHandle, "w" );  
	*stderr = *fp;  
	setvbuf( stderr, NULL, _IONBF, 0 );

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	CGameMain GameMain;

	return GameMain.Run(ALCHEMY_NULL);
}