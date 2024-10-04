// include this define to create a set of rulers on the edges of the screen.
#define PrintDebugRulers

#include "GameScene.h"
#include "GameObject.h"

using namespace FC_Console;

void GameScene::initaliseScene ( int lastGameSceneResponse )
{
	m_runLoop = true;

	if ( !isInitialised )
	{
		isInitialised = true;

		m_stdWindow.setWindow ( 160, 50 );

		m_frontBuffer = new ScreenBuffer ( 160, 50 );
		m_backBuffer = new ScreenBuffer ( 160, 50 );

		// seed the random number generator with the current time.
		std::srand ( static_cast< unsigned int >( std::time ( 0 ) ) );
	}

}

void GameScene::processInput ( )
{
	if ( GetConsoleWindow ( ) == GetForegroundWindow ( ) )
	{
		// only check for keyboard input when our window is the foreground window.
		//m_keyboardInput.tick ( m_deltaTime );
	}
	else
	{
		//m_keyboardInput.resetKeyPresses ( );
	}
}

void GameScene::updateScene ( )
{
}

/// <summary>
/// Helper method that draws a GameObject to the back buffer.
/// </summary>
/// <param name="object">The object to draw.</param>
void GameScene::drawGameObject ( const GameObject & object )
{
	if (!object.getActive())
	{
		// don't draw non-active objects.
		return;
	}

	m_backBuffer->applyRenderData ( object.getGridX ( ), object.getGridY ( ),
									object.getWidth ( ), object.getHeight ( ),
									object.draw ( ) );
}

void GameScene::renderFrame ( )
{
	// swap front and back buffer
	auto buffer = m_frontBuffer;
	m_frontBuffer = m_backBuffer;
	m_backBuffer = buffer;

#ifdef PrintDebugRulers

	// Draw Debug rows
	for (int i = 0; i < getScreenHeight( ); i++)
	{
		char buffer[4];

		sprintf_s(buffer, "%03d", i);

		m_backBuffer->setCharColour(0, i, CellColour::Fore_Black, CellColour::Back_White);
		m_backBuffer->setChar(0, i, buffer[0]);
		m_backBuffer->setChar(1, i, buffer[1]);
		m_backBuffer->setChar(2, i, buffer[2]);

		m_backBuffer->setCharColour(getScreenWidth() - 1, i, CellColour::Fore_Black, CellColour::Back_White);
		m_backBuffer->setChar(getScreenWidth() - 3, i, buffer[0]);
		m_backBuffer->setChar(getScreenWidth() - 2, i, buffer[1]);
		m_backBuffer->setChar(getScreenWidth() - 1, i, buffer[2]);
	}

	for (int i = 0; i < getScreenWidth( ); i++)
	{
		m_backBuffer->setCharColour(i, 0, CellColour::Fore_Black, CellColour::Back_White);
		m_backBuffer->setCharColour(i, getScreenHeight()-1, CellColour::Fore_Black, CellColour::Back_White);

		if (i % 10 != 0)
		{
			continue;
		}

		char buffer[4];

		sprintf_s(buffer, "%03d", i);

		m_backBuffer->setChar(i, 0, buffer[0]);
		m_backBuffer->setChar(i + 1, 0, buffer[1]);
		m_backBuffer->setChar(i + 2, 0, buffer[2]);

		m_backBuffer->setChar(i,     getScreenHeight() - 1, buffer[0]);
		m_backBuffer->setChar(i + 1, getScreenHeight() - 1, buffer[1]);
		m_backBuffer->setChar(i + 2, getScreenHeight() - 1, buffer[2]);
	}

#endif

	// render the back buffer to the screen.
	m_frontBuffer->displayBuffer();
}

void GameScene::processFrame ( )
{
	processInput ( );
	updateScene ( );
	drawScene ( );
	renderFrame ( );
}

