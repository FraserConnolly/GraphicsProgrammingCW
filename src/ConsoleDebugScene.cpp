#include "ConsoleDebugScene.h"
#include "Input.h"

int ConsoleDebugScene::loadNextScene ( std::shared_ptr<GameScene> & newScene, bool & loadAdditively )
{
    loadAdditively = false;
    newScene = nullptr;
    return 0;
}

void ConsoleDebugScene::initaliseLevel ( )
{ 
    auto colOffset = 4;
    auto rowOffset = 3;
    auto counter = 0;

    _positionTextbox.setGridPosition ( 4, rowOffset+counter );
    _positionTextbox.setActive ( true );

    FC_Console::GameObject * ptr;

    ptr = &_positionTextbox;

    _objects[counter++] = ptr ;

    _rotationTextbox.setGridPosition ( 4, rowOffset + counter );
    _rotationTextbox.setActive ( true );
    _objects[ counter ++] = &_rotationTextbox;

    rowOffset++;

    _mousePositionTextbox.setGridPosition ( 4, rowOffset + counter );
    _mousePositionTextbox.setActive ( true );
    _objects [ counter++ ] = ( &_mousePositionTextbox );

    _mousePositionDeltaTextbox.setGridPosition ( colOffset, rowOffset + counter );
    _mousePositionDeltaTextbox.setActive (true);
    _objects [ counter++ ] = ( &_mousePositionDeltaTextbox );

    _mouseWheelTextbox.setGridPosition ( colOffset, rowOffset + counter );
    _mouseWheelTextbox.setActive (true);
    _objects [ counter++ ] = ( &_mouseWheelTextbox );

}

void ConsoleDebugScene::drawScene ( )
{
    for ( auto object : _objects )
    {
        if ( object == nullptr )
        {
            continue;
        }

        drawGameObject ( *object );
    }
}

void ConsoleDebugScene::updateScene ( )
{ 

    if ( _monitor != nullptr )
    {
        const glm::vec3 & position = _monitor->GetPosition ( );
        const glm::vec3 & rotation = _monitor->GetRotationEuler ( );
        _positionTextbox.updateText ( position.x, position.y, position.z );
        _rotationTextbox.updateText ( rotation.x, rotation.y, rotation.z );
    }

    _mousePositionTextbox.updateText ( Input::GetMousePosition ( ).x, Input::GetMousePosition ( ).y );
    _mousePositionDeltaTextbox.updateText ( Input::GetMouseDelta ( ).x, Input::GetMouseDelta ( ).y );
    _mouseWheelTextbox.updateText ( Input::GetMouseWheelDelta ( ).x, Input::GetMouseWheelDelta ( ).y );
}

void ConsoleDebugScene::initaliseScene ( int lastSceneReturnCode )
{ 
    GameScene::initaliseScene ( lastSceneReturnCode );
    initaliseLevel ( );
}
