#include "D3DUtils.h"
#include "Image.h"
#include <DirectXTexEXR.h>
#include <directxtk/DDSTextureLoader.h>
#include <iostream>
#include <memory>
#include <vector>

// D3DUtils::D3DUtils() : mDXGIAdapter(nullptr), mDXGIDevice1(nullptr),
// mDXGIFactory1(nullptr) {} D3DUtils::~D3DUtils() {
//	std::cout << "D3DUtils closed.\n";
// }

/**
 * .
 *
 * \param mainWindow
 * \param device
 * \param context
 * \param swapChain
 * \param backBuffer
 * \return
 */

ComPtr<ID3D11Texture2D>
CreateStagingTexture(ComPtr<ID3D11Device> &device,
                     ComPtr<ID3D11DeviceContext> &context, const int width,
                     const int height, const std::vector<uint8_t> &image,
                     const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
                     const int mipLevels = 1, const int arraySize = 1) {

    // 스테이징 텍스춰 만들기
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = mipLevels;
    txtDesc.ArraySize = arraySize;
    txtDesc.Format = pixelFormat;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_STAGING;
    txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ComPtr<ID3D11Texture2D> stagingTexture;
    if (FAILED(device->CreateTexture2D(&txtDesc, NULL,
                                       stagingTexture.GetAddressOf()))) {
        std::cout << "Failed()" << std::endl;
    }

    // CPU에서 이미지 데이터 복사
    size_t pixelSize = sizeof(uint8_t) * 4;
    if (pixelFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) {
        pixelSize = sizeof(uint16_t) * 4;
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
    uint8_t *pData = (uint8_t *)ms.pData;
    for (UINT h = 0; h < UINT(height); h++) { // 가로줄 한 줄씩 복사
        memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize],
               width * pixelSize);
    }
    context->Unmap(stagingTexture.Get(), NULL);

    return stagingTexture;
}

static void CompileShader(const std::wstring filename,
                          const std::string profile,
                          ComPtr<ID3DBlob> &shaderBlob,
                          ComPtr<ID3DBlob> &errorBlob) {
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ThrowIfFailed(D3DCompileFromFile(filename.c_str(), nullptr,
                                     D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                     profile.c_str(), compileFlags, 0,
                                     &shaderBlob, &errorBlob),
                  errorBlob.Get());
}

bool D3DUtils::InitD3D(HWND &mainWindow, ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       ComPtr<IDXGISwapChain> &swapChain,
                       ComPtr<ID3D11Buffer> &backBuffer) {

    const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
    UINT createDeviceFlags = 0;

    const D3D_FEATURE_LEVEL featureLevels[2] = {D3D_FEATURE_LEVEL_11_0,
                                                D3D_FEATURE_LEVEL_9_3};
    D3D_FEATURE_LEVEL featureLevel;

    if (FAILED(D3D11CreateDevice(nullptr, driverType, 0, createDeviceFlags,
                                 featureLevels, ARRAYSIZE(featureLevels),
                                 D3D11_SDK_VERSION, device.GetAddressOf(),
                                 &featureLevel, context.GetAddressOf()))) {
        std::cout << "D3D11CreateDevice() failed." << std::endl;
        return false;
    }
    return true;
}

bool D3DUtils::CreateDevice(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context) {
    const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif

    const D3D_FEATURE_LEVEL featureLevels[2] = {D3D_FEATURE_LEVEL_11_0,
                                                D3D_FEATURE_LEVEL_9_3};
    D3D_FEATURE_LEVEL featureLevel;

    ThrowIfFailed(D3D11CreateDevice(nullptr, driverType, 0, createDeviceFlags,
                                    featureLevels, ARRAYSIZE(featureLevels),
                                    D3D11_SDK_VERSION, device.GetAddressOf(),
                                    &featureLevel, context.GetAddressOf()));

    std::cout << "featureLevel : " << featureLevel << std::endl;

    return true;
}

bool D3DUtils::CreateSwapChain(ComPtr<ID3D11Device> &device,
                               ComPtr<IDXGISwapChain> &swapChain,
                               HWND &mainWindow, int width, int height) {
    ComPtr<IDXGIFactory1> mDXGIFactory1;
    ComPtr<IDXGIDevice1> mDXGIDevice1;
    ComPtr<IDXGIAdapter> mDXGIAdapter;
    device.As(&mDXGIDevice1);
    mDXGIDevice1->GetAdapter(&mDXGIAdapter);

    // std::cout << "device : " << device.Get() << std::endl;
    // std::cout << "DXGIDevice1 : " << mDXGIDevice1.Get() << std::endl;

    mDXGIAdapter->GetParent(__uuidof(IDXGIFactory1), &mDXGIFactory1);

    UINT numQualityLevels = 0;
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4,
                                          &numQualityLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferCount = 1;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = mainWindow;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Windowed = TRUE;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    std::cout << "numQuarlityLevels : " << numQualityLevels << std::endl;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    ThrowIfFailed(mDXGIFactory1->CreateSwapChain(mDXGIDevice1.Get(), &sd,
                                                 swapChain.GetAddressOf()));

    return true;
}

void D3DUtils::CreateViewport(ComPtr<ID3D11DeviceContext> &context,
                              D3D11_VIEWPORT &viewport, int width, int height) {
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = float(width);
    viewport.Height = float(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);
}

void D3DUtils::CreateRasterizerState(ComPtr<ID3D11Device> &device,
                                     ComPtr<ID3D11RasterizerState> &rs) {
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    // rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    // rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    ThrowIfFailed(device->CreateRasterizerState(&rastDesc, rs.GetAddressOf()));
}

void D3DUtils::CreateDSS(ComPtr<ID3D11Device> &device,
                         ComPtr<ID3D11DepthStencilState> &dss) {
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, dss.GetAddressOf()));
}

void D3DUtils::CreateDepthBuffers(
    ComPtr<ID3D11Device> &device,
    ComPtr<ID3D11DepthStencilView> &depthStencilView, int width, int height) {

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    UINT numQualityLevels = 0;
    device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_UNORM, 4,
                                          &numQualityLevels);

    bool useMSAA = true;
    if (useMSAA && numQualityLevels > 0) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = numQualityLevels - 1;
    } else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    ThrowIfFailed(
        device->CreateTexture2D(&desc, 0, depthStencilBuffer.GetAddressOf()));
    ThrowIfFailed(device->CreateDepthStencilView(
        depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf()));
}

void D3DUtils::CreateIndexBuffer(ComPtr<ID3D11Device> &device,
                                 const std::vector<uint32_t> &indices,
                                 ComPtr<ID3D11Buffer> &indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.StructureByteStride = sizeof(uint32_t);

    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
    indexBufferData.pSysMem = indices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    ThrowIfFailed(device->CreateBuffer(&bufferDesc, &indexBufferData,
                                       indexBuffer.GetAddressOf()));
}

// todo: move D3D11_INPUT_ELEMENT_DESC into param
void D3DUtils::CreateInputLayout(ComPtr<ID3D11Device> &device,
                                 ComPtr<ID3D11InputLayout> &inputLayout,
                                 ComPtr<ID3DBlob> &shaderBlob) {
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    ThrowIfFailed(
        device->CreateInputLayout(inputDesc.data(), UINT(inputDesc.size()),
                                  shaderBlob->GetBufferPointer(),
                                  shaderBlob->GetBufferSize(), &inputLayout));
}

void D3DUtils::CreateVertexShader(ComPtr<ID3D11Device> &device,
                                  const std::wstring &filename,
                                  ComPtr<ID3D11VertexShader> &vertexShader,
                                  ComPtr<ID3D11InputLayout> &inputLayout) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    CompileShader(filename, "vs_5_0", shaderBlob, errorBlob);
    ThrowIfFailed(device->CreateVertexShader(shaderBlob->GetBufferPointer(),
                                             shaderBlob->GetBufferSize(), NULL,
                                             vertexShader.GetAddressOf()));
    D3DUtils::CreateInputLayout(device, inputLayout, shaderBlob);
}

void D3DUtils::CreatePixelShader(ComPtr<ID3D11Device> &device,
                                 const std::wstring &filename,
                                 ComPtr<ID3D11PixelShader> &pixelShader) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;
    CompileShader(filename, "ps_5_0", shaderBlob, errorBlob);

    ThrowIfFailed(device->CreatePixelShader(shaderBlob->GetBufferPointer(),
                                            shaderBlob->GetBufferSize(), NULL,
                                            pixelShader.GetAddressOf()));
}

void D3DUtils::CreateTexture(ComPtr<ID3D11Device> &device,
                             ComPtr<ID3D11DeviceContext> &context,
                             const std::string &filename,
                             ComPtr<ID3D11Texture2D> &texture,
                             ComPtr<ID3D11ShaderResourceView> &textureSRV) {
    int width, height, channels;

    // unsigned char *img =
    //     stbi_load(filename.c_str(), &width, &height, &channels, 0);

    unsigned char *img = Image::Load(filename.c_str(), width, height, channels);
    // std::cout << img << std::endl;
    std::vector<uint8_t> image;
    image.resize(width * height * 4);

    if (channels == 1) {
        for (size_t i = 0; i < width * height; i++) {
            uint8_t g = img[i * channels + 0];
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = g;
            }
        }
    } else if (channels == 2) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 2; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
            image[4 * i + 2] = 255;
            image[4 * i + 3] = 255;
        }
    } else if (channels == 3) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 3; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
            image[4 * i + 3] = 255;
        }
    } else if (channels == 4) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
        }
    } else {
        std::cout << "Cannot read " << channels << " channels" << std::endl;
    }
    std::cout << filename.c_str() << " channels : " << channels << std::endl;

    Image::FreeImage(img);

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA d;
    d.pSysMem = image.data();
    d.SysMemPitch = desc.Width * sizeof(uint8_t) * 4;

    device->CreateTexture2D(&desc, &d, texture.GetAddressOf());
    device->CreateShaderResourceView(texture.Get(), nullptr,
                                     textureSRV.GetAddressOf());
}

void D3DUtils::CreateDDSTexture(
    ComPtr<ID3D11Device> &device, const wchar_t *filename, bool isCubeMap,
    ComPtr<ID3D11ShaderResourceView> &textureResourceView) {

    ComPtr<ID3D11Texture2D> texture;

    UINT miscFlags = 0;
    if (isCubeMap) {
        miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    std::wcout << (std::wstring)filename << std::endl;

    ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(
        device.Get(), filename, 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE, 0, miscFlags,
        DirectX::DDS_LOADER_FLAGS(false),
        (ID3D11Resource **)texture.GetAddressOf(),
        textureResourceView.GetAddressOf(), NULL));
}
