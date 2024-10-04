#pragma once
#include "Console/GameScene.h"
#include "Console/UiText.h"
#include "Transform.h"

//using namespace FC_Console;

class ConsoleDebugScene :
    public FC_Console::GameScene
{

protected:

    // Inherited via GameScene
    void initaliseLevel ( ) override;
    void drawScene ( ) override;
    void updateScene ( ) override;

public:

    void initaliseScene ( int ) override;

    int loadNextScene ( std::shared_ptr<FC_Console::GameScene> & newScene, bool & loadAdditively ) override;

    void SetTransformToMonitor ( const Transform & transform )
    {
        _monitor = &transform;
    }


private:

    FC_Console::UiText _positionTextbox = FC_Console::UiText ( "Position (%06.2f, %06.2f, %06.2f)", 100 ) ;
    FC_Console::UiText _rotationTextbox = FC_Console::UiText ( "Rotation (%06.2f, %06.2f, %06.2f)", 100 ) ;
    FC_Console::UiText _mousePositionTextbox = FC_Console::UiText ( "Mouse Position (%06d, %06d)", 100 ) ;
    FC_Console::UiText _mousePositionDeltaTextbox = FC_Console::UiText ( "Mouse Delta (%06d, %06d)", 100 ) ;
    FC_Console::UiText _mouseWheelTextbox = FC_Console::UiText ( "Wheel Delta (%06d, %06d)", 100 ) ;

    FC_Console::GameObject * _objects [ 5 ];

    const Transform * _monitor = nullptr;

};

