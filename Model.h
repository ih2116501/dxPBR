#pragma once
#include "Mesh.h"
#include "ObjectBase.h"
#include <memory>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

struct Materials {
    float metallic;
    float roughness;
    float dummy[2];
};

class Model : ObjectBase {
  public:
    Model();
    ~Model();

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                    MeshData &meshData) override;

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                               ComPtr<ID3D11DeviceContext> &context) override;

    void Render(ComPtr<ID3D11DeviceContext> &context) override;

    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> textureSRV;
    ComPtr<ID3D11SamplerState> mSamplerState;
    ComPtr<ID3D11SamplerState> mClampSampler;
    std::vector<ID3D11SamplerState *> mSamplerList;

    Materials mMaterialData;
    ComPtr<ID3D11Buffer> mPixelCB;
    std::vector<ID3D11ShaderResourceView *> mResViews;
    std::vector<Mesh> mMeshes;

  private:
};