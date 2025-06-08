#include "stdafx.h"
#include "Player.h"
#include "shader.h"
#include "Camera.h"
#include "InputManager.h"

void CInputManager::ProcessingMouseMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
}

void CInputManager::ProcessingKeyboardMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)	{
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;
			case VK_RETURN:
				break;
			case VK_CONTROL:
				break;
			default:
				break;
			}
			break;
		default:
			break;
	}
}

LRESULT CInputManager::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)	{
		case WM_ACTIVATE:
			break;
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			ProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			ProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}
	return(0);
}
// =========================


void CSpaceShipInputManager::ProcessInput(HWND& hwnd, CObjectShader* shader)
{
	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		if (dwDirection) shader->PlayerMove(dwDirection, 0.15f);
	}

	if (GetCapture() == hwnd)
	{
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - old_cursor_pos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - old_cursor_pos.y) / 3.0f;
		SetCursorPos(old_cursor_pos.x, old_cursor_pos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				shader->PlayerRotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else
				shader->PlayerRotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}
}

void CSpaceShipInputManager::ProcessingMouseMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&old_cursor_pos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CSpaceShipInputManager::ProcessingKeyboardMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_CONTROL:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CSpaceShipInputManager::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID) {
	case WM_ACTIVATE:
		break;
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		ProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		ProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

// ====================== 
void CStartInputManager::ProcessInput(HWND& hwnd, CObjectShader* shader)
{
	
}

void CStartInputManager::ProcessingMouseMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{

}

void CStartInputManager::ProcessingKeyboardMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
}

LRESULT CStartInputManager::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID) {
	case WM_ACTIVATE:
		break;
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		ProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		ProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

// ======================
void CRollerCoasterInputManager::ProcessInput(HWND& hwnd, CObjectShader* shader)
{

	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		if (dwDirection) shader->PlayerMove(dwDirection, 0.15f);
	}

	if (GetCapture() == hwnd)
	{
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - old_cursor_pos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - old_cursor_pos.y) / 3.0f;
		SetCursorPos(old_cursor_pos.x, old_cursor_pos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				shader->PlayerRotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else
				shader->PlayerRotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}
}

void CRollerCoasterInputManager::ProcessingMouseMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&old_cursor_pos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CRollerCoasterInputManager::ProcessingKeyboardMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case 'n':
		case 'N':
			//IsNextshader = true;
			break;
		case VK_CONTROL:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CRollerCoasterInputManager::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{

	switch (nMessageID) {
	case WM_ACTIVATE:
		break;
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		ProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		ProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CTankInputManager::ProcessInput(HWND& hwnd, CObjectShader* shader)
{
	static UCHAR pKeyBuffer[256];
	if (GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		if (dwDirection) {
			shader->PlayerMove(dwDirection, 0.15f);
		}
	}

	//if (togle) {
	//	if (togle & DIR_S) {
	//	}
	//	if (togle & DIR_A) {
	//	}
	//}

	if (GetCapture() == hwnd)
	{
		SetCursor(NULL);
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		float cxMouseDelta = (float)(ptCursorPos.x - old_cursor_pos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - old_cursor_pos.y) / 3.0f;
		SetCursorPos(old_cursor_pos.x, old_cursor_pos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			if (pKeyBuffer[VK_LBUTTON] & 0xF0)
				shader->PlayerRotate(0.0f, cxMouseDelta, 0.0f);
		}
	}

	/*if (isFire) {
		if (togle & DIR_A) {
			lock_object = shader->PickObjectPointedByCursor(LOWORD(old_cursor_pos.x), HIWORD(old_cursor_pos.x));
		}
		else {
			lock_object = nullptr;
		}
		dynamic_cast<CTankPlayer*>(shader->player.get())->FireBullet(lock_object);
		isFire = false;
	}*/

}

void CTankInputManager::ProcessingMouseMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		::GetCursorPos(&old_cursor_pos);
		isFire = true;
		break;
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&old_cursor_pos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CTankInputManager::ProcessingKeyboardMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case 's':
		case 'S':
			togle ^= DIR_S;
			break;
		case 'a':
		case 'A':
			togle ^= DIR_A;
			break;
		case VK_RETURN:
			break;
		case VK_CONTROL:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CTankInputManager::ProcessingWindowMessage(HWND& hWnd, UINT& nMessageID, WPARAM& wParam, LPARAM& lParam)
{
	switch (nMessageID) {
	case WM_ACTIVATE:
		break;
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		ProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		ProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}
