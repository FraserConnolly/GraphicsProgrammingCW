#include "Button.h"

using namespace FC_Console;

Button::Button ( ) : GameObject ( 0, 0 )
{ 
	m_active = false;
}

Button::~Button ( )
{ 
	if ( m_symbol != nullptr )
	{
		delete [ ] m_symbol;
	}
}


void Button::setSelectionStatus ( const bool selected )
{
	if ( selected == m_selected )
	{
		return;
	}

	m_selected = selected;

	if ( m_symbol == nullptr )
	{
		return;
	}

	for ( unsigned int i = 0; i < m_width * m_height; i++ )
	{
		m_symbol [ i ].Attributes = m_selected ? m_selectedColour : m_neutralColour;
	}
}

void Button::setCharacters ( const WCHAR * characters, size_t columns, size_t rows )
{ 
	setCharacters ( characters, columns, rows, 0, 0 );
}

void Button::setCharacters ( const WCHAR * characters, size_t columns, size_t rows, size_t horizontalMargin, size_t verticalMargin )
{ 
	m_horizontalMargin = horizontalMargin;
	m_verticalMargin   = verticalMargin;
	m_textWidth  = columns;
	m_textHeight = rows;
	m_width  = ( unsigned int ) ( columns + ( horizontalMargin * 2 ) );
	m_height = ( unsigned int ) ( rows +    ( verticalMargin   * 2 ) );

	if ( m_symbol != nullptr )
	{
		delete [ ] m_symbol;
	}

	m_symbol = new RenderCellData [ m_width * m_height ];

	for ( int i = 0; i < (int) ( m_width * m_height ); i++ )
	{
		m_symbol [ i ].UnicodeChar = ' ';
		m_symbol [ i ].Attributes = m_selected ? m_selectedColour : m_neutralColour;
	}

	size_t marginOffset = m_verticalMargin * m_width;

	for ( size_t r = 0, i = 0; r < m_textHeight; r++ )
	{
		for ( size_t c = 0; c < m_textWidth; c++, i++ )
		{
			m_symbol [ marginOffset + m_horizontalMargin + i ].UnicodeChar = characters [ i ];
		}
		marginOffset += m_horizontalMargin * 2;
	}

}
