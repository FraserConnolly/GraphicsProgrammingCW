#pragma once
#include <iostream>
#include <string>
#include "Window.h"
#include "ScreenBuffer.h"

namespace FC_Console
{

	class GameObject;

	class GameScene
	{
	public:
		GameScene ( )
			: m_stdWindow ( GetStdHandle ( STD_OUTPUT_HANDLE ) )
		{ };

		virtual ~GameScene ( )
		{
			delete m_backBuffer;
			delete m_frontBuffer;

			// restore the screen buffer to the standard output.
			SetConsoleActiveScreenBuffer ( GetStdHandle ( STD_OUTPUT_HANDLE ) );
		}

		virtual void initaliseScene ( int lastGameSceneResponse );
		void processFrame ( );
		virtual int loadNextScene ( std::shared_ptr<GameScene> & newScene, bool & loadAdditively ) = 0;

		inline const int getScreenWidth ( ) const
		{
			return m_stdWindow.getWidth ( );
		}

		inline const int getScreenHeight ( ) const
		{
			return m_stdWindow.getHeight ( );
		}

	protected:

		virtual void initaliseLevel ( ) = 0;
		virtual void processInput ( );
		virtual void updateScene ( );
		virtual void drawScene ( ) = 0;

		/// <summary>
		/// Helper method that takes in the pointer to an array of GameObjects
		/// and the number of objects in the array.
		/// Each active object is then rendered to the back buffer.
		/// </summary>
		template< class GameObject >
		void drawObjectsInArray ( GameObject * objects, size_t count )
		{
			for ( int i = 0; i < count; i++ )
			{
				GameObject & gObject = objects [ i ];

				if ( !gObject.getActive ( ) )
				{
					continue;
				}

				drawGameObject ( gObject );
			}
		}

		void drawGameObject ( const GameObject & object );

		bool m_runLoop = false;

		/// <summary>
		/// This screen buffer pointed to by m_backBuffer 
		/// will never be the active console buffer.
		/// </summary>
		ScreenBuffer * m_backBuffer = nullptr;

	private:

		void renderFrame ( );

		bool isInitialised = false;

		ScreenBuffer * m_frontBuffer = nullptr;
		Window m_stdWindow;

	};

}