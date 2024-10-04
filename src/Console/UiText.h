#pragma once
#include "GameObject.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace FC_Console
{

	class UiText :
		public GameObject
	{
	public:
		UiText ( const char * format, const unsigned int maxLength ) :
			GameObject ( 0, 1 ), m_format ( nullptr ), m_length ( maxLength ), m_string ( nullptr )
		{
			// initialise m_symbol with spaces
			m_symbol = new RenderCellData [ maxLength ];
			for ( unsigned int i = 0; i < maxLength; i++ )
			{
				m_symbol [ i ].AsciiChar = ' ';
				m_symbol [ i ].Attributes = CellColour::Fore_White ;
			}

			// initialise string holder with nulls
			m_string = new char [ maxLength ];
			memset ( m_string, '\0', maxLength );

			// initialise format string holder as a C style string.
			auto formatLength = strlen ( format ) + 1; // +1 is to accommodate the null character at the end of C style strings.
			m_format = new char [ formatLength ];
			strcpy_s ( m_format, formatLength, format );
		}

		~UiText ( )
		{
			delete[ ] m_symbol;
			delete[ ] m_string;
			delete[ ] m_format;
		}

		template<typename... Args> void updateText ( Args... args )
		{
			if ( m_format == nullptr )
			{
				return;
			}

			m_width = sprintf_s ( m_string, m_length, m_format, args... );

			for ( size_t i = 0; i < m_width; i++ )
			{
				m_symbol [ i ].AsciiChar = m_string [ i ];
			}
		}

		// Inherited via GameObject
		const pRenderCellData draw ( ) const override
		{
			return m_active ? ( pRenderCellData ) m_symbol : nullptr;
		};

	private:

		pRenderCellData m_symbol;
		char * m_string;
		size_t m_length;
		char * m_format;

	};

}