#include "GUIManager.h"
#include <memory>
GUIManager *gGUIManager = nullptr;

GUIManager::GUIManager(int width, int height)
    : mScreenWidth(width), mScreenHeight(height), mLButtonDown(false),
      mRButtonDown(false), mLDragStart(false), mRDragStart(false), mMouseX(0),
      mMouseY(0), mMouseMove(false) {}
GUIManager::~GUIManager() { 
    gGUIManager = nullptr;
    std::cout << "GUIManager closed.\n"; }


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return gGUIManager->MsgProc(hWnd, msg, wParam, lParam);
}

bool GUIManager::InitWindows(HWND &mainWindow) {
    gGUIManager = this;
    WNDCLASSEX wc = {sizeof(WNDCLASSEX),    CS_CLASSDC, WndProc, 0L,   0L,
                     GetModuleHandle(NULL), NULL,       NULL,    NULL, NULL,
                     L"DxSoftRenderer",     NULL};

    if (!RegisterClassEx(&wc)) {
        std::cout << "RegisterClassEx() failed." << std::endl;
        return false;
    }

    RECT wr = {0, 0, mScreenWidth, mScreenHeight};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
    mainWindow = CreateWindow(
        wc.lpszClassName, L"PBR Renderer", WS_OVERLAPPEDWINDOW, 100, 100,
        wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, wc.hInstance, NULL);

    if (!mainWindow) {
        std::cout << "CreateWindow() failed." << std::endl;
        return false;
    }

    ShowWindow(mainWindow, SW_SHOWDEFAULT);
    UpdateWindow(mainWindow);
    return true;
}

LRESULT GUIManager::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
        if (!mLButtonDown)
            mLDragStart = true;
        mLButtonDown = true;
        std::cout << "lbtndown" << std::endl;
        break;
    case WM_RBUTTONDOWN:
        if (!mRButtonDown)
            mRDragStart = true;
        mRButtonDown = true;
        std::cout << "rbtndown" << std::endl;
        break;
    case WM_LBUTTONUP:
        std::cout << "lbtnup" << std::endl;
        mLButtonDown = false;
        break;
    case WM_RBUTTONUP:
        mRButtonDown = false;
        break;
    case WM_MOUSEMOVE:
        mMouseX = LOWORD(lParam);
        mMouseY = HIWORD(lParam);
        mMouseMove = true;

        break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool GUIManager::InitGUI(HWND &mainWindow, ComPtr<ID3D11Device> &device,
                         ComPtr<ID3D11DeviceContext> &context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(mScreenWidth, mScreenHeight);
    ImGui::StyleColorsLight();

    if (!ImGui_ImplWin32_Init(mainWindow)) {
        return false;
    }

    if (!ImGui_ImplDX11_Init(device.Get(), context.Get())) {
        return false;
    }
    return true;
}
