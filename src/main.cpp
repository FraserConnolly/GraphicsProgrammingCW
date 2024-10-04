#include <iostream>
#include "GameEngine.h"

#ifndef _DEBUG
#include <windows.h>
#endif

#if _DEBUG
int main(int argc, char** argv) //argument used to call SDL main
#else
int APIENTRY WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#endif
{
	GameEngine mainGame;
	mainGame.run ( );

	return 0;
}