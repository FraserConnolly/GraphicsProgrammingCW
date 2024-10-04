#pragma once

#include <cmath>

namespace FC_Console
{

#define FC_PI 3.14159265358979323846

	typedef struct _vector2Int
	{
		_vector2Int ( )
			: X ( 0 ), Y ( 0 )
		{ }

		_vector2Int ( const int x, const int y )
			: X ( x ), Y ( y )
		{ }

	#pragma region Operators

	#pragma region Equality operators

		inline bool operator == ( const _vector2Int & other ) const
		{
			return X == other.X && Y == other.Y;
		}

		inline bool operator != ( const _vector2Int & other ) const
		{
			return !( operator==( other ) );
		}

	#pragma endregion

	#pragma region Arethmetic operators

		inline _vector2Int & operator *=( const int v )
		{
			X *= v;
			Y *= v;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2Int operator*( const int v ) const
		{
			_vector2Int p;

			p.X = X * v;
			p.Y = Y * v;

			return p;
		}

		inline _vector2Int & operator +=( const _vector2Int & v )
		{
			X += v.X;
			Y += v.Y;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2Int operator+( const _vector2Int & v ) const
		{
			_vector2Int p;

			p.X = X + v.X;
			p.Y = Y + v.Y;

			return p;
		}

		inline _vector2Int & operator -=( const _vector2Int & v )
		{
			X -= v.X;
			Y -= v.Y;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2Int operator-( const _vector2Int & v ) const
		{
			_vector2Int p;

			p.X = X - v.X;
			p.Y = Y - v.Y;

			return p;
		}

		inline _vector2Int & operator +=( const int v )
		{
			X += v;
			Y += v;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2Int operator+( const int v ) const
		{
			_vector2Int p;

			p.X = X + v;
			p.Y = Y + v;

			return p;
		}

		inline _vector2Int & operator -=( const int v )
		{
			X -= v;
			Y -= v;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2Int operator-( const int v ) const
		{
			_vector2Int p;

			p.X = X - v;
			p.Y = Y - v;

			return p;
		}

	#pragma endregion

	#pragma endregion

		int X;
		int Y;

	} Vector2Int, * pVector2Int;

	typedef struct _vector2
	{
		_vector2 ( )
			: X ( 0.0f ), Y ( 0.0f )
		{ }

		_vector2 ( const float x, const float y )
			: X ( x ), Y ( y )
		{ }

	#pragma region Operators

	#pragma region Equality operators

		inline bool operator == ( const _vector2 & other ) const
		{
			return X == other.X && Y == other.Y;
		}

		inline bool operator != ( const _vector2 & other ) const
		{
			return !( operator==( other ) );
		}

	#pragma endregion

	#pragma region Arethmetic operators

		inline _vector2 & operator +=( const _vector2 & v )
		{
			X += v.X;
			Y += v.Y;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2 operator+( const _vector2 & v ) const
		{
			_vector2 p;

			p.X = X + v.X;
			p.Y = Y + v.Y;

			return p;
		}

		inline _vector2 & operator -=( const _vector2 & v )
		{
			X -= v.X;
			Y -= v.Y;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2 operator-( const _vector2 & v ) const
		{
			_vector2 p;

			p.X = X - v.X;
			p.Y = Y - v.Y;

			return p;
		}

		inline _vector2 & operator *=( const float v )
		{
			X *= v;
			Y *= v;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2 operator*( const float v ) const
		{
			_vector2 p;

			p.X = X * v;
			p.Y = Y * v;

			return p;
		}

		inline _vector2 & operator +=( const float v )
		{
			X += v;
			Y += v;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2 operator+( const float v ) const
		{
			_vector2 p;

			p.X = X + v;
			p.Y = Y + v;

			return p;
		}

		inline _vector2 & operator -=( const float v )
		{
			X -= v;
			Y -= v;

			// Assignment operator returns left side of assignment.
			return *this;
		}

		inline _vector2 operator-( const float v ) const
		{
			_vector2 p;

			p.X = X - v;
			p.Y = Y - v;

			return p;
		}

	#pragma endregion

	#pragma endregion

	#pragma region Normalisation

		float getMagnitude ( ) const
		{
			return std::sqrt ( X * X + Y * Y );
		}

		float getMagnitudeSqr ( ) const
		{
			return X * X + Y * Y;
		}

		_vector2 getNormalised ( ) const
		{
			if ( X == 0 && Y == 0 )
			{
				return Vector2 ( 0, 0 );
			}

			// Calculate the magnitude of the vector
			float magnitude = getMagnitude ( );

			// Normalize the vector by dividing each component by the magnitude
			Vector2 normalizedVector;
			normalizedVector.X = X / magnitude;
			normalizedVector.Y = Y / magnitude;

			return normalizedVector;
		}

		void normalise ( )
		{
			_vector2 n = getNormalised ( );

			X = n.X;
			Y = n.Y;
		}

		void rotateVector ( float angleInDegree )
		{
			// Convert degrees to radians
			double angleToRotate = ( angleInDegree * ( FC_PI / 180.0f ) );

			rotateRadians ( angleToRotate );
		}

		/// <param name="angleInRadians">Angle by which to change the direction (in radians)</param>
		void rotateRadians ( double angleInRadians )
		{
			X = static_cast< float > ( X * std::cos ( angleInRadians ) - Y * std::sin ( angleInRadians ) );
			Y = static_cast< float > ( X * std::sin ( angleInRadians ) + Y * std::cos ( angleInRadians ) );
		}

	#pragma endregion

		float X;
		float Y;

	} Vector2, * pVector2;

}
