#pragma once

#include "Camera.h"
#include "GUIManager.h"
#include "Light.h"
#include "Model.h"
#include "ConstantData.h"
#include "RenderManager.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <wrl/client.h>


class Engine {
  public:
    Engine();
    ~Engine();

    bool InitEngine(ComPtr<ID3D11Device> &device);
    bool Run();
    bool Update();
    bool Render();

    // std::shared_ptr<GUIManager> mGUIManager;
    std::shared_ptr<GUIManager> mGUIManager;
    //GUIManager gman;
    // std::shared_ptr<D3DUtils> mD3DUtils;
    // D3DUtils mD3DUtils;

  private:
    std::shared_ptr<RenderManager> mRenderManager;

    HWND mMainWindow;
    int mScreenWidth;
    int mScreenHeight;
    ComPtr<ID3D11Device> mDevice;
    ComPtr<ID3D11DeviceContext> mContext;
    ComPtr<IDXGISwapChain> mSwapChain;
    ComPtr<IDXGISwapChain1> mSwapChain1;
    ComPtr<ID3D11RenderTargetView> mBackBufferRTV;
    ComPtr<ID3D11DepthStencilView> mDepthStencilView;
    ComPtr<ID3D11Buffer> mConstantBuffer;
    ComPtr<ID3D11RasterizerState> mBasicRS;
    D3D11_VIEWPORT mViewport;
    ComPtr<ID3D11DepthStencilState> mDSS;
    std::shared_ptr<Camera> mCamera;
    ComPtr<ID3D11VertexShader> mVertexShader;
    ComPtr<ID3D11VertexShader> mSkyBoxVS;
    ComPtr<ID3D11PixelShader> mPixelShader;
    ComPtr<ID3D11PixelShader> mSkyBoxPS;
    ComPtr<ID3D11InputLayout> mInputLayout;
    ComPtr<ID3D11InputLayout> mSkyBoxIL;
    DirectX::SimpleMath::Vector3 mEyePos;
    DirectX::SimpleMath::Vector2 mViewRot;
    ConstData constData;

    std::shared_ptr<Model> mMainModel;
    std::shared_ptr<Model> mScreenSquare;
    std::shared_ptr<Model> mSkyBox;
    
    std::vector<std::shared_ptr<Model>> objList;
    DirectX::SimpleMath::Vector2 dMouse;
    DirectX::SimpleMath::Vector2 mPrevMouseXY;

    int mModelNum;
    bool mModelChangeFlag;
};