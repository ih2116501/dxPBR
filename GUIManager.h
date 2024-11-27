#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class GUIManager {
  public:
    GUIManager(int width, int height);
    ~GUIManager();

    bool InitWindows(HWND &mainWindow);
    bool InitGUI(HWND &mainWindow, ComPtr<ID3D11Device> &device,
                 ComPtr<ID3D11DeviceContext> &context);

    int mScreenWidth, mScreenHeight;
    int mMouseX, mMouseY;
    bool mLButtonDown, mRButtonDown;
    bool mLDragStart, mRDragStart;
    bool mMouseMove;
    
    LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};