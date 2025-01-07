#include "Model.h"
#include "D3DUtils.h"
#include <iostream>

using namespace DirectX::SimpleMath;

Model::Model() {}

Model::~Model() {}

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       std::vector<MeshData> &meshes) {
    mMeshes.clear();
    mMeshes.shrink_to_fit();
    mPixelConstData = mEmptyPixelConstData;

    if (mSamplerList.size() == 0)
        this->CreateSamplers(device);
    this->CreateMeshes(device, context, meshes);
    mPixelConstData.useWireframe = 0;

    if (mPixelCB.Get() == nullptr)
        D3DUtils::CreateConstantBuffer(device, mPixelConstData, mPixelCB);
}

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       MeshData &meshData) {
    mMeshes.clear();
    mMeshes.shrink_to_fit();
    mPixelConstData = mEmptyPixelConstData;

    if (mSamplerList.size() == 0)
        this->CreateSamplers(device);
    Mesh newMesh;
    // D3DUtils::CreateTexture(device, "d", texture, textureSRV);
    this->CreateMesh(device, context, meshData);
    mPixelConstData.useWireframe = 0;
    if (mPixelCB.Get() == nullptr)
        D3DUtils::CreateConstantBuffer(device, mPixelConstData, mPixelCB);
}

void Model::CreateSamplers(ComPtr<ID3D11Device> &device) {
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
    mSamplerList.push_back(mSamplerState.Get());

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&sampDesc, mClampSampler.GetAddressOf());
    mSamplerList.push_back(mClampSampler.Get());
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {}

void Model::CreateMeshes(ComPtr<ID3D11Device> &device,
                         ComPtr<ID3D11DeviceContext> &context,
                         std::vector<MeshData> &meshes) {
    for (auto &meshData : meshes) {
        this->CreateMesh(device, context, meshData);
    }
}

void Model::CreateMesh(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       MeshData &meshData) {
    Mesh newMesh;

    // D3DUtils::CreateTexture(device, "d", texture, textureSRV);
    if (!meshData.albedoTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context, meshData.albedoTextureFilename,
                                newMesh.albedoTexture, newMesh.albedoSRV);
    }

    if (!meshData.emissiveTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context,
                                meshData.emissiveTextureFilename,
                                newMesh.emissiveTexture, newMesh.emissiveSRV);
    }

    if (!meshData.normalTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context, meshData.normalTextureFilename,
                                newMesh.normalTexture, newMesh.normalSRV);
    }

    if (!meshData.heightTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context, meshData.heightTextureFilename,
                                newMesh.heightTexture, newMesh.heightSRV);
    }

    if (!meshData.aoTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context, meshData.aoTextureFilename,
                                newMesh.aoTexture, newMesh.aoSRV);
    }
    if (!meshData.metallicRoughnessFilename.empty()) {
        mPixelConstData.useMetallicRoughness = 1;
        D3DUtils::CreateTexture(
            device, context, meshData.metallicRoughnessFilename,
            newMesh.metallicRoughnessTexture, newMesh.metallicRoughnessSRV);
    }
    if (!meshData.metallicTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context,
                                meshData.metallicTextureFilename,
                                newMesh.metallicTexture, newMesh.metallicSRV);
    }

    if (!meshData.roughnessTextureFilename.empty()) {
        D3DUtils::CreateTexture(device, context,
                                meshData.roughnessTextureFilename,
                                newMesh.roughnessTexture, newMesh.roughnessSRV);
    }

    D3DUtils::CreateVertexBuffer(device, meshData.vertices,
                                 newMesh.mVertexBuffer);
    D3DUtils::CreateIndexBuffer(device, meshData.indices, newMesh.mIndexBuffer);
    newMesh.indexCount = meshData.indices.size();
    mMeshes.push_back(newMesh);
}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {

    // for (int i = 0; i < mMeshes.size(); i++) {
    for (auto &mesh : mMeshes) {
        ID3D11ShaderResourceView *vsSRVs[1] = {mesh.heightSRV.Get()};
        mResViews = {mesh.albedoSRV.Get(),
                     mesh.normalSRV.Get(),
                     mesh.aoSRV.Get(),
                     mesh.metallicSRV.Get(),
                     mesh.roughnessSRV.Get(),
                     mesh.emissiveSRV.Get(),
                     mesh.metallicRoughnessSRV.Get()};

        const std::vector<ID3D11ShaderResourceView *> psNullSRVs(
            mResViews.size(), nullptr);
        const std::vector<ID3D11ShaderResourceView *> vsNullSRVs(1, nullptr);

        context->IASetIndexBuffer(mesh.mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT,
                                  0);
        context->IASetVertexBuffers(0, 1, mesh.mVertexBuffer.GetAddressOf(),
                                    &mesh.stride, &mesh.offset);
        context->VSSetShaderResources(0, 1, vsSRVs);
        context->VSSetSamplers(0, mSamplerList.size(), mSamplerList.data());
        context->PSSetSamplers(0, mSamplerList.size(), mSamplerList.data());
        context->PSSetConstantBuffers(5, 1, mPixelCB.GetAddressOf());
        context->PSSetShaderResources(10, mResViews.size(), mResViews.data());
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->DrawIndexed(mesh.indexCount, 0, 0);

        // unbinding
        context->VSSetShaderResources(0, vsNullSRVs.size(), vsNullSRVs.data());
        context->PSSetShaderResources(10, psNullSRVs.size(), psNullSRVs.data());
    }
}
