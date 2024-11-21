#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

struct Pipeline {
    ComPtr<ID3D11InputLayout> InputLayout;
    ComPtr<ID3D11RasterizerState> rs;
    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    std::vector<ID3D11ShaderResourceView *> SRVs;
    std::vector<ID3D11Buffer *> vsCbList;
    std::vector<ID3D11Buffer *> psCbList;
    ComPtr<ID3D11DepthStencilView> DepthStencilView;
    ComPtr<ID3D11DepthStencilState> DSS;

    void SetPipeline(ComPtr<ID3D11DeviceContext> &context);

};
