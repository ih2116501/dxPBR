#include "RenderManager.h"
#include "D3DUtils.h"
#include "Model.h"
#include "ObjectBase.h"
#include <algorithm>
#include <directxtk/PostProcess.h>
#include <functional>
#include <unordered_map>

RenderManager::RenderManager(int width, int height)
    : mScreenWidth(width), mScreenHeight(height), mUseWireframe(false) {}

RenderManager::~RenderManager() {}

void RenderManager::InitRenderer(ComPtr<ID3D11Device> &device,
                                 ComPtr<ID3D11DeviceContext> &context,
                                 ComPtr<IDXGISwapChain> &swapchain) {

    mDevice = device;
    mContext = context;
    mSwapchain = swapchain;
    CreateRasterizerStates();
    CreateBuffers();

    std::wstring basePath = L"./Assets/Textures/Cubemap/";
    
    // create textures
    D3DUtils::CreateDDSTexture(
        mDevice, (basePath + L"outdoorEnvHDR.dds").c_str(), true, mEnvSRV);
    mSkyboxSRVs.push_back(mEnvSRV.Get());

    D3DUtils::CreateDDSTexture(mDevice,
                               (basePath + L"outdoorSpecularHDR.dds").c_str(),
                               true, mSpecularSRV);
    mSkyboxSRVs.push_back(mSpecularSRV.Get());

    D3DUtils::CreateDDSTexture(mDevice,
                               (basePath + L"outdoorDiffuseHDR.dds").c_str(),
                               true, mIrradianceSRV);
    mSkyboxSRVs.push_back(mIrradianceSRV.Get());

    D3DUtils::CreateDDSTexture(mDevice, (basePath + L"outdoorBrdf.dds").c_str(),
                               false, mBrdfSRV);
    mSkyboxSRVs.push_back(mBrdfSRV.Get());

    // create shaders                              
    D3DUtils::CreateVertexShader(mDevice, L"VertexShader.hlsl", mVertexShader,
                                 mInputLayout);
    D3DUtils::CreatePixelShader(mDevice, L"PixelShader.hlsl", mPixelShader);
    D3DUtils::CreateVertexShader(mDevice, L"SkyBoxVS.hlsl", mSkyboxVS,
                                 mSkyboxIL);
    D3DUtils::CreatePixelShader(mDevice, L"SkyBoxPS.hlsl", mSkyboxPS);
    D3DUtils::CreateVertexShader(mDevice, L"ScreenVS.hlsl", mScreenVS,
                                 mScreenIL);
    D3DUtils::CreatePixelShader(mDevice, L"ScreenPS.hlsl", mScreenPS);
    D3DUtils::CreatePixelShader(mDevice, L"ToneMapPS.hlsl", mTonemapPS);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    // sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&sampDesc, mClampSampler.GetAddressOf());

    D3DUtils::CreateDepthBuffers(mDevice, mDSV, mScreenWidth, mScreenHeight);
    D3DUtils::CreateDSS(mDevice, mDSS);

    // set pipelines
    mBasicPipeline.rs = mBasicRS;
    //mBasicPipeline.rs = mWireframeRS;
    mBasicPipeline.InputLayout = mInputLayout;
    mBasicPipeline.vs = mVertexShader;
    mBasicPipeline.ps = mPixelShader;
    mBasicPipeline.vsCbList;
    mBasicPipeline.SRVs = mSkyboxSRVs;
    mBasicPipeline.psCbList;
    mBasicPipeline.DepthStencilView = mDSV;
    mBasicPipeline.DSS = mDSS;

    mSkyBoxPipeline.rs = mSkyBoxRS;
    mSkyBoxPipeline.InputLayout = mSkyboxIL;
    mSkyBoxPipeline.vs = mSkyboxVS;
    mSkyBoxPipeline.ps = mSkyboxPS;
    mSkyBoxPipeline.SRVs = mSkyboxSRVs;
    mSkyBoxPipeline.vsCbList;
    mSkyBoxPipeline.psCbList;
    mSkyBoxPipeline.DepthStencilView = mDSV;
    mSkyBoxPipeline.DSS = mDSS;

    mScreenPipeline = mBasicPipeline;
    mScreenPipeline.rs = mBasicRS;
    mScreenPipeline.vs = mScreenVS;
    mScreenPipeline.ps = mScreenPS;

    //std::unordered_map<OjbectType, std::shared_ptr<std::vector<Model>>> um;
    //std::shared_ptr<std::vector<Model>> defaultobjList;
    //defaultobjList = std::make_shared<std::vector<Model>>();

    //um[DEFAULT_OBJECT] = defaultobjList;
    //std::shared_ptr<std::vector<Model>> list;

    //list = um[DEFAULT_OBJECT];
    // this.objectList = objlist;
    /*
     *foreach(auto obj : objlist){
     *
     *   um.find(obj.objectType).push_back(obj);
     * }
     */
}

void RenderManager::CreateRasterizerStates() {
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    ThrowIfFailed(
        mDevice->CreateRasterizerState(&rastDesc, mBasicRS.GetAddressOf()));

    //rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = true;
    ThrowIfFailed(
        mDevice->CreateRasterizerState(&rastDesc, mSkyBoxRS.GetAddressOf()));

    rastDesc.FillMode = D3D11_FILL_WIREFRAME;
    rastDesc.CullMode = D3D11_CULL_NONE;
    ThrowIfFailed(
        mDevice->CreateRasterizerState(&rastDesc, mWireframeRS.GetAddressOf()));
}

void RenderManager::CreateBuffers() {
    UINT numQualityLevels = 0;
    ThrowIfFailed(mDevice->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &numQualityLevels));

    ComPtr<ID3D11Texture2D> backBuffer;
    ThrowIfFailed(
        mSwapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    ThrowIfFailed(mDevice->CreateRenderTargetView(
        backBuffer.Get(), NULL, mBackBufferRTV.GetAddressOf()));

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.MipLevels = desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = 0;
    if (numQualityLevels) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = numQualityLevels - 1;
    } else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }

    ThrowIfFailed(
        mDevice->CreateTexture2D(&desc, NULL, mFloatBuffer.GetAddressOf()));

    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    ThrowIfFailed(
        mDevice->CreateTexture2D(&desc, NULL, mResolvedBuffer.GetAddressOf()));

    ThrowIfFailed(mDevice->CreateRenderTargetView(mFloatBuffer.Get(), NULL,
                                                  mFloatRTV.GetAddressOf()));
    ThrowIfFailed(mDevice->CreateShaderResourceView(
        mResolvedBuffer.Get(), NULL, mResolvedSRV.GetAddressOf()));
    ThrowIfFailed(mDevice->CreateShaderResourceView(mFloatBuffer.Get(), NULL,
                                                    mFloatSRV.GetAddressOf()));
}

void RenderManager::ClearFrame() {
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    mContext->ClearRenderTargetView(mBackBufferRTV.Get(), clearColor);
    mContext->ClearDepthStencilView(
        mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    mBasicPipeline.rs = mUseWireframe ? mWireframeRS : mBasicRS;
}

void RenderManager::RenderObjects() {
    mContext->OMSetDepthStencilState(mDSS.Get(), 0);
    mContext->OMSetRenderTargets(1, mFloatRTV.GetAddressOf(), mDSV.Get());

    mBasicPipeline.SetPipeline(mContext);
}

void RenderManager::RenderSkybox() {
    mContext->OMSetDepthStencilState(mDSS.Get(), 0);
    mContext->OMSetRenderTargets(1, mFloatRTV.GetAddressOf(), mDSV.Get());

    mSkyBoxPipeline.SetPipeline(mContext);
}

void RenderManager::RenderToneMap() {
    mContext->PSSetShader(mTonemapPS.Get(), nullptr, 0);
    mContext->PSSetShaderResources(0, 1, mFloatSRV.GetAddressOf());
}

void RenderManager::RenderScreen() {
    mScreenPipeline.SetPipeline(mContext);
    mContext->ResolveSubresource(mResolvedBuffer.Get(), 0, mFloatBuffer.Get(),
                                 0, DXGI_FORMAT_R16G16B16A16_FLOAT);

    mContext->PSSetShaderResources(0, 1, mResolvedSRV.GetAddressOf());
    mContext->OMSetRenderTargets(1, mBackBufferRTV.GetAddressOf(), nullptr);
}