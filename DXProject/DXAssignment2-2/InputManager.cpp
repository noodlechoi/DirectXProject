#include "stdafx.h"
#include "InputManager.h"
#include "Object.h"

extern HWND ghWnd;

// command
void ClickCommand::Execute(CObject& object)
{

}

// Input
InputManager::InputManager()
	: mouse{new ClickCommand}
{
}

InputManager::~InputManager()
{
	if (mouse)	delete mouse;
}

void InputManager::ProcessInput()
{
	if (GetKeyboardState(key_buffer)) {
	}
	if (GetCapture() == ghWnd) {
		if (key_buffer[VK_LBUTTON] & 0xF0) {
			mouse->Execute(*object);
		}
	}
}