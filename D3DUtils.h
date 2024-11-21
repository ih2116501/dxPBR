#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

static void ThrowIfFailed(HRESULT hr, ID3DBlob *errorBlob = nullptr) {
    if (FAILED(hr)) {
        // Set a breakpoint on this line to catch DirectX API errors
        if ((D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }
        if (errorBlob) {
            std::cout << "Shader compile error\n"
                      << (char *)errorBlob->GetBufferPointer() << std::endl;
        }
        throw std::exception();
    }
}

namespace D3DUtils {
bool InitD3D(HWND &mainWindow, ComPtr<ID3D11Device> &device,
             ComPtr<ID3D11DeviceContext> &context,
             ComPtr<IDXGISwapChain> &swapChain,
             ComPtr<ID3D11Buffer> &backBuffer);

bool CreateDevice(ComPtr<ID3D11Device> &device,
                  ComPtr<ID3D11DeviceContext> &context);
bool CreateSwapChain(ComPtr<ID3D11Device> &device,
                     ComPtr<IDXGISwapChain> &swapChain, HWND &mainWindow,
                     int width, int height);

void CreateViewport(ComPtr<ID3D11DeviceContext> &context,
                    D3D11_VIEWPORT &viewport, int width, int height);

template <typename T_VERTEX>
void CreateVertexBuffer(ComPtr<ID3D11Device> &device,
                        const std::vector<T_VERTEX> &vertices,
                        ComPtr<ID3D11Buffer> &vertexBuffer) {

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.StructureByteStride = sizeof(T_VERTEX);

    D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
    vertexBufferData.pSysMem = vertices.data();
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    ThrowIfFailed(device->CreateBuffer(&bufferDesc, &vertexBufferData,
                                       vertexBuffer.GetAddressOf()));
}

template <typename T_CONSTANTDATA>
void CreateConstantBuffer(ComPtr<ID3D11Device> &device,
                          const T_CONSTANTDATA &constantData,
                          ComPtr<ID3D11Buffer> &constantBuffer) {

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ByteWidth = sizeof(constantData);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = &constantData;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    ThrowIfFailed(
        device->CreateBuffer(&desc, &initData, constantBuffer.GetAddressOf()));
}

template <typename T_DATA>
static void UpdateBuffer(const ComPtr<ID3D11Device> &device,
                         const ComPtr<ID3D11DeviceContext> &context,
                         const T_DATA &bufferData,
                         ComPtr<ID3D11Buffer> &buffer) {

    if (!buffer) {
        std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, &bufferData, sizeof(bufferData));
    context->Unmap(buffer.Get(), NULL);
}

void CreateDDSTexture(ComPtr<ID3D11Device> &device, const wchar_t *filename,
                      bool isCubeMap,
                      ComPtr<ID3D11ShaderResourceView> &textureResourceView);

void CreateRasterizerState(ComPtr<ID3D11Device> &device,
                           ComPtr<ID3D11RasterizerState> &rs);

void CreateDSS(ComPtr<ID3D11Device> &device,
               ComPtr<ID3D11DepthStencilState> &dss);
void CreateDepthBuffers(ComPtr<ID3D11Device> &device,
                        ComPtr<ID3D11DepthStencilView> &depthStencilView,
                        int width, int height);

void CreateIndexBuffer(ComPtr<ID3D11Device> &device,
                       const std::vector<uint32_t> &indices,
                       ComPtr<ID3D11Buffer> &indexBuffer);

void CreateInputLayout(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11InputLayout> &inputLayout,
                       ComPtr<ID3DBlob> &shaderBlob);

void CreateVertexShader(ComPtr<ID3D11Device> &device,
                        const std::wstring &filename,
                        ComPtr<ID3D11VertexShader> &vertexShader,
                        ComPtr<ID3D11InputLayout> &inputLayout);

void CreatePixelShader(ComPtr<ID3D11Device> &device,
                       const std::wstring &filename,
                       ComPtr<ID3D11PixelShader> &pixelShader);

void CreateTexture(ComPtr<ID3D11Device> &device,
                   ComPtr<ID3D11DeviceContext> &context,
                   const std::string &filename,
                   ComPtr<ID3D11Texture2D> &texture,
                   ComPtr<ID3D11ShaderResourceView> &textureSRV);

}; // namespace D3DUtils