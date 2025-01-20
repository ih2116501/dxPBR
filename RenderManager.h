#pragma once
#include "Pipeline.h"
#include <string>

class RenderManager {
  public:
    RenderManager(int width, int height);
    ~RenderManager();

    void InitRenderer(ComPtr<ID3D11Device> &device,
                      ComPtr<ID3D11DeviceContext> &context,
                      ComPtr<IDXGISwapChain> &swapchain);

    // template <typename T_OBJ> void Render(T_OBJ obj) {
    //     obj.setPipeline();
    //     obj.Render(mPipelineState);
    // }

    void RenderObjects();
    void RenderSkybox();
    void RenderToneMap();
    void RenderScreen();
    void ClearFrame();
    bool mUseWireframe;
    bool mOption1;


  private:
    void CreateRasterizerStates();
    void CreateBuffers();

    int mScreenWidth, mScreenHeight;

    ComPtr<ID3D11ShaderResourceView> mEnvSRV;
    ComPtr<ID3D11ShaderResourceView> mSpecularSRV;
    ComPtr<ID3D11ShaderResourceView> mIrradianceSRV;
    ComPtr<ID3D11ShaderResourceView> mBrdfSRV;
    ComPtr<IDXGISwapChain> mSwapchain;

    ComPtr<ID3D11Device> mDevice;
    ComPtr<ID3D11DeviceContext> mContext;

    ComPtr<ID3D11RasterizerState> mBasicRS;
    ComPtr<ID3D11RasterizerState> mSkyBoxRS;
    ComPtr<ID3D11RasterizerState> mWireframeRS;

    ComPtr<ID3D11VertexShader> mVertexShader;
    ComPtr<ID3D11PixelShader> mPixelShader;
    ComPtr<ID3D11VertexShader> mSkyboxVS;
    ComPtr<ID3D11PixelShader> mSkyboxPS;
    ComPtr<ID3D11HullShader> mHullShader;
    ComPtr<ID3D11DomainShader> mDomainShader;
    ComPtr<ID3D11InputLayout> mSkyboxIL;
    ComPtr<ID3D11InputLayout> mInputLayout;
    ComPtr<ID3D11InputLayout> mScreenIL;
    ComPtr<ID3D11SamplerState> mSamplerState;
    ComPtr<ID3D11SamplerState> mClampSampler;
    

    ComPtr<ID3D11VertexShader> mScreenVS;
    ComPtr<ID3D11PixelShader> mScreenPS;
    ComPtr<ID3D11PixelShader> mTonemapPS;

    ComPtr<ID3D11DepthStencilState> mDSS;
    ComPtr<ID3D11DepthStencilView> mDSV;

    ComPtr<ID3D11Texture2D> mFloatBuffer;
    ComPtr<ID3D11RenderTargetView> mBackBufferRTV;
    ComPtr<ID3D11RenderTargetView> mFloatRTV;
    ComPtr<ID3D11ShaderResourceView> mResolvedSRV;
    ComPtr<ID3D11ShaderResourceView> mFloatSRV;
    
    
    ComPtr<ID3D11Texture2D> mResolvedBuffer;

    std::vector<ID3D11ShaderResourceView *> mSkyboxSRVs;

    Pipeline mBasicPipeline;
    Pipeline mSkyBoxPipeline;
    Pipeline mScreenPipeline;
};
