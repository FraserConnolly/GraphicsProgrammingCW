#pragma once

#include "Point.h"
#include "RenderCellData.h"

namespace FC_Console
{

	/// <summary>
	/// This is the underlying class for all game objects that exist on the console grid.
	/// 
	/// Coordinates:
	/// Each game object exits on a grid however it uses a float data type to store its location.
	/// This is to allow an object to move by less than a grid unit each frame.
	/// </summary>
	class GameObject
	{
	public:

		const int CELL_WIDTH = 1;
		const int CELL_HEIGHT = 2;

		GameObject ( const unsigned int width, const unsigned int height )
			: m_width ( width ), m_height ( height ),
			m_X ( 0 ), m_Y ( 0 ), m_gridX ( 0 ), m_gridY ( 0 ),
			m_active ( true )
		{ }


		/// <summary>
		/// 
		/// </summary>
		/// <returns>
		/// An array of CHAR_INFO which is of size m_width * m_height.
		/// The array starts at the objects grid position then goes along the width.
		/// Then each row starts again at the first X position.
		/// Will return nullptr if nothing is to be drawn.
		/// </returns>
		virtual const pRenderCellData draw ( ) const = 0;

		bool inPosition ( const int x, const int y ) const
		{
			return ( x == getGridX ( ) && y == getGridY ( ) );
		}

		bool inPosition ( const Vector2Int position )
		{
			return inPosition ( position.X, position.Y );
		}

		bool hasCollided ( const GameObject & coll ) const
		{
			// Concept comes from https://www.geeksforgeeks.org/find-two-rectangles-overlap/
			// Note:
			//	the area check has been changed to > rather than == because it is perfectly valid
			//		in this game for an object to only consume one grid position 
			//		in which case the l and r values would be equal.
			//	the height check has been changed to < rather than > because the Y axis in
			//		this game is inverted.

			// represent the two objects as rectangles using the top left and bottom right corners
			// note that height and width must have -1 as anything with a dimension less than 1 will not occupy a grid space.
			Vector2Int l1 ( m_gridX, m_gridY );
			Vector2Int r1 ( m_gridX + ( int ) ( m_width - 1 ), m_gridY + ( int ) ( m_height - 1 ) );

			Vector2Int l2 ( coll.m_gridX, coll.m_gridY );
			Vector2Int r2 ( coll.m_gridX + ( int ) ( coll.m_width - 1 ), coll.m_gridY + ( int ) ( coll.m_height - 1 ) );

			if ( l1.X > r1.X || l1.Y > r1.Y || l2.X > r2.X || l2.Y > r2.Y )
			{
				// either self or the other object has an area of 0 so there can be no collision.
				return false;
			}

			if ( l1.X > r2.X || l2.X > r1.X )
			{
				// one rectangle is on left side of the other
				return false;
			}

			if ( r1.Y < l2.Y || r2.Y < l1.Y )
			{
				// one rectangle is above the other
				return false;
			}

			return true;
		}

		/// <summary>
		/// This must be overridden by base classes that can be collided with.
		/// </summary>
		/// <param name="collision">The other object in the collision.</param>
		/// <param name="collisionPoint">The grid point where the collision occurred.</param>
		virtual void onCollision ( const GameObject & collision, const Vector2Int & collisionPoint )
		{ }

	#pragma region Position Get/Set

		const unsigned int getHeight ( ) const
		{
			return m_height;
		}

		const unsigned int getWidth ( ) const
		{
			return m_width;
		}

		inline const float getX ( ) const
		{
			return m_X;
		}

		inline const int getGridX ( ) const
		{
			return m_gridX;
		}

		inline const float getY ( ) const
		{
			return m_Y;
		}

		inline const int getGridY ( ) const
		{
			return m_gridY;
		}

		inline const Vector2Int getGridPosition ( ) const
		{
			return Vector2Int ( m_gridX, m_gridY );
		}

		inline void setX ( const float x )
		{
			m_X = x;
			m_gridX = int ( x / CELL_WIDTH );
		}

		inline void setGridX ( const int x )
		{
			m_X = float ( x * CELL_WIDTH );
			m_gridX = x;
		}

		inline void setY ( const float y )
		{
			m_Y = y;
			m_gridY = int ( y / CELL_HEIGHT );
		}

		inline void setGridY ( const int y )
		{
			m_Y = float ( y * CELL_HEIGHT );
			m_gridY = y;
		}

		inline void setGridPosition ( const int x, const int y )
		{
			setGridX ( x );
			setGridY ( y );
		}

		inline void setGridPosition ( const Vector2Int & position )
		{
			setGridPosition ( position.X, position.Y );
		}

		inline void translateByGridUnit ( const Vector2 & translation )
		{
			translate ( translation.X, translation.Y );
		}

		inline void translate ( const Vector2 v )
		{
			translate ( v.X, v.Y );
		}

		inline void translate ( const float x, const float y )
		{
			setX ( m_X + x );
			setY ( m_Y + y );
		}

		inline void translateByGridUnit ( const Vector2Int & translation )
		{
			translateByGridUnit ( translation.X, translation.Y );
		}

		inline void translateByGridUnit ( const int x, const int y )
		{
			setX ( m_X + ( x * CELL_WIDTH ) );
			setY ( m_Y + ( y * CELL_HEIGHT ) );
		}

	#pragma endregion

		void setActive ( bool active )
		{
			m_active = active;
		}

		const bool getActive ( ) const
		{
			return m_active;
		}

	protected:

		unsigned int m_height;
		unsigned int m_width;
		bool m_active;

	private:

		float m_X;
		int m_gridX;
		float m_Y;
		int m_gridY;

	};

}