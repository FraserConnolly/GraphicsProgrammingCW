#include <iostream>
#include "GameEngine.h"

#ifndef _DEBUG
#include <windows.h>
#endif

#if _DEBUG
int main(int argc, char ** argv) //argument used to call SDL main
#else
int APIENTRY WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#endif
{
	GameEngine mainGame;
	
	const wchar_t * path = L"DefaultGame.fgame";

#if _DEBUG
	if ( argc >= 2 ) {
		std::wstring warg ( argv [ 1 ] , argv [ 1 ] + strlen ( argv [ 1 ] ) );
		path = warg.c_str ( );
	}
#else
	LPWSTR * argv;
	int argc;
	argv = CommandLineToArgvW ( GetCommandLineW ( ) , &argc );

	if ( argc >= 2 ) {
		std::wstring arg1 = argv [ 1 ]; // Convert wide string to std::wstring
		path = arg1.c_str();
	}
#endif

	// load the game
	int result = mainGame.loadGame ( path );
	if ( result != 0 )
	{
		std::cout << "Error loading game: " << result << std::endl;
		return result;
	}

	mainGame.run ( );

	return 0;
}