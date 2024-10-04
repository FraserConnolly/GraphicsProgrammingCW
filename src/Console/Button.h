#pragma once
#include "GameObject.h"

typedef wchar_t WCHAR;

namespace FC_Console
{

    class Button :
        public GameObject
    {

    public:

        Button ( );
        ~Button ( );

        void setSelectionStatus ( const bool selected );
        void setCharacters ( const WCHAR * characters, size_t columns, size_t rows );
        void setCharacters ( const WCHAR * characters, size_t columns, size_t rows, size_t horizontalMargin, size_t verticalMargin );
        void setColours ( const CellColour neutralColour, const CellColour selectedColour )
        {
            m_neutralColour = neutralColour;
            m_selectedColour = selectedColour;
        }

        // Inherited via GameObject
        const pRenderCellData draw ( ) const override
        {
            return m_symbol;
        };

    private:

        bool m_selected = false;
        size_t m_textWidth = 0;
        size_t m_textHeight = 0;
        size_t m_horizontalMargin = 0;
        size_t m_verticalMargin = 0;
        CellColour m_selectedColour = ( CellColour ) ( CellColour::Back_Magenta | CellColour::Fore_Black );
        CellColour m_neutralColour = ( CellColour ) ( CellColour::Back_Black | CellColour::Fore_White );
        pRenderCellData m_symbol = nullptr;

    };

}