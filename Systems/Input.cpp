#include "Input.h"
#include "../Entities/Player.h"
#include "../Game/Camera.h"

void HandleInput()
{
    UpdatePlayer();
    
    if (IsKeyPressed(KEY_R))
        ResetCamera();
}