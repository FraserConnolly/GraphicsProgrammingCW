#pragma once
#include "Window.h"
#include "RenderCellData.h"

namespace FC_Console
{

	class ScreenBuffer
	{
	public:

		ScreenBuffer ( const int x, const int y );

		~ScreenBuffer ( );

		WCHAR getChar ( const int x, const int y ) const;
		void setChar ( const int x, const int y, char c );
		void setChar ( const int x, const int y, WCHAR c );
		void setCharColour ( const int x, const int y, CellColour cellColour );
		void setCharColour ( const int x, const int y, CellColour foreground, CellColour background );
		void setBackgroundColour ( const CellColour background );
		void setForegroundColour ( const CellColour foreground );
		void applyRenderData ( const int x, const int y, const unsigned int width, const unsigned int height, const pRenderCellData data );

		/// <summary>
		/// Sets all characters in the buffer to the space character i.e. ' '.
		/// </summary>
		void clearBuffer ( );

		/// <summary>
		/// Copies the contents of the local buffer to the output buffer
		/// </summary>
		void displayBuffer ( );

	private:
		int m_row;
		int m_col;
		HANDLE m_bufferHandle;
		WORD m_defaultAttribute;
		Window * m_window;
		CHAR_INFO * m_buffer;
		CHAR_INFO ** m_rowPtr;
	};

}