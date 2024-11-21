#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Windows.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class GUIManager {
public:
	
	GUIManager(int width, int height);
	~GUIManager();
	 
	bool InitWindows(HWND& mainWindow);
	bool InitGUI(HWND& mainWindow, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);

	int mScreenWidth, mScreenHeight;
	LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};