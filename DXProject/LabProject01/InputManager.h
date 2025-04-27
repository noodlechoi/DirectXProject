#pragma once

class CInputManager
{
public:
	void ProcessingMouseMessage(HWND , UINT , WPARAM , LPARAM );
	void ProcessingKeyboardMessage(HWND , UINT , WPARAM , LPARAM );
	LRESULT ProcessingWindowMessage(HWND , UINT , WPARAM , LPARAM );
};

