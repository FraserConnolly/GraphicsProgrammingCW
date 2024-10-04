#include "ScreenBuffer.h"

#define DEFAULT_ATTRIBUTE CellColour::Fore_White | CellColour::Back_Black

#pragma region Constructors, destructors, and operators

using namespace FC_Console;

/// <summary>
/// Sets all characters in the buffer to the space character i.e. ' '.
/// </summary>
ScreenBuffer::ScreenBuffer ( const int x, const int y )
	: m_col ( x ),
	m_row ( y ),
	m_buffer ( nullptr ),
	m_rowPtr ( nullptr ),
	m_bufferHandle ( nullptr ),
	m_defaultAttribute ( DEFAULT_ATTRIBUTE )
{
	// allocate a single continuous block of memory big enough to story the whole screen buffer.
	m_buffer = new CHAR_INFO [ m_row * m_col ];

	// establish pointers to each row in the continuous block of memory.
	// this allows for get/sets to operate as if they have a 2D array.
	m_rowPtr = new CHAR_INFO * [ m_row ];
	for ( int i = 0; i < m_row; i++ )
	{
		m_rowPtr [ i ] = &m_buffer [ i * m_col ];
	}

	m_bufferHandle = CreateConsoleScreenBuffer (
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL );

	if ( m_bufferHandle == INVALID_HANDLE_VALUE )
	{
		throw;
	}

	if ( !SetConsoleActiveScreenBuffer ( m_bufferHandle ) )
	{
		cout << "SetConsoleActiveScreenBuffer failed with error " << GetLastError ( ) << endl;
		throw;
	}

	m_window = new Window ( m_bufferHandle );
	m_window->setWindow ( m_col, m_row );

	clearBuffer ( );
}

// destructor
ScreenBuffer::~ScreenBuffer ( )
{
	delete [ ] m_buffer;
	delete [ ] m_rowPtr;
	delete m_window;
}

#pragma endregion

#pragma region Getters / Setters

WCHAR ScreenBuffer::getChar ( const int x, const int y ) const
{
	return m_rowPtr [ y ][ x ].Char.UnicodeChar;
}

void ScreenBuffer::setChar ( const int x, const int y, char c )
{
	m_rowPtr [ y ][ x ].Char.AsciiChar = c;
}

void ScreenBuffer::setChar ( const int x, const int y, WCHAR c )
{
	m_rowPtr [ y ][ x ].Char.UnicodeChar = c;
}

#pragma endregion

#pragma region Colours

void ScreenBuffer::setCharColour ( const int x, const int y, const CellColour cellColour )
{
	m_rowPtr [ y ][ x ].Attributes &= ~0x0077;
	m_rowPtr [ y ][ x ].Attributes |= ( cellColour );
}

void ScreenBuffer::setCharColour ( const int x, const int y, const CellColour foreground, const CellColour background )
{
	setCharColour ( x, y, ( CellColour ) ( foreground | background ) );
}

void ScreenBuffer::setBackgroundColour ( CellColour background )
{
	m_defaultAttribute &= ~Back_White;
	m_defaultAttribute |= background;
}

void ScreenBuffer::setForegroundColour ( CellColour foreground )
{
	m_defaultAttribute &= ~Fore_White;
	m_defaultAttribute |= foreground;
}

void ScreenBuffer::applyRenderData ( const int x, const int y, const unsigned int width, const unsigned int height, const pRenderCellData data )
{ 
	if (x < 0 || y < 0 || x >= m_window->getWidth() || y >= m_window->getHeight())
	{
		return;
	}

	if ( data == nullptr )
	{
		return;
	}

	int dataIndex = 0;
	for ( size_t h = 0; h < height ; h++ )
	{
		for ( size_t w = 0; w < width ; w++ )
		{
			m_rowPtr [ y + h ][ x + w ].Char.UnicodeChar = data [ dataIndex    ].UnicodeChar;
			m_rowPtr [ y + h ][ x + w ].Attributes       = data [ dataIndex ++ ].Attributes;
		}
	}
}

#pragma endregion

void ScreenBuffer::clearBuffer ( )
{
	CHAR_INFO defaultInfo = { ' ', m_defaultAttribute };

	int length = m_row * m_col;

	for ( int i = 0; i < length ; i++ )
	{
		m_buffer [ i ] = defaultInfo;
	}
}

/// <summary>
/// Copies the contents of the local buffer to the output buffer.
/// Then makes the output buffer active.
/// The local buffer is then cleared.
/// </summary>
void ScreenBuffer::displayBuffer ( )
{
	if ( m_bufferHandle == nullptr )
	{
		throw;
		return;
	}

	COORD dwBufferSize = { (short) m_col, (short) m_row };

	COORD dwBufferCoord = { 0, 0 };

	SMALL_RECT writeRegion = { 0, 0, 0, 0 };
	writeRegion.Top    = 0;
	writeRegion.Left   = 0;
	writeRegion.Bottom = m_row - 1;
	writeRegion.Right  = m_col - 1;

	auto result = WriteConsoleOutput (
		m_bufferHandle,
		m_buffer,
		dwBufferSize,
		dwBufferCoord,
		&writeRegion
	);

	if ( !result )
	{
		printf ( "WriteConsoleOutput  failed - (%d)\n", GetLastError ( ) );
		throw;
	}

	SetConsoleActiveScreenBuffer ( m_bufferHandle );

	clearBuffer ( );
}
