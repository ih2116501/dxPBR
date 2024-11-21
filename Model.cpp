#include "Model.h"
#include "D3DUtils.h"
#include <DirectXMath.h>
#include <iostream>

using namespace DirectX::SimpleMath;

Model::Model() { std::cout << "model open\n"; }

Model::~Model() { std::cout << "model closed\n"; }

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       MeshData &meshData) {
    // ObjectType = DEFAULT_MODEL;
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    // sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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
    mSamplerList.push_back(mSamplerState.Get());
    mSamplerList.push_back(mClampSampler.Get());
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

    mResViews = {newMesh.albedoSRV.Get(),    newMesh.normalSRV.Get(),
                 newMesh.aoSRV.Get(),        newMesh.metallicSRV.Get(),
                 newMesh.roughnessSRV.Get(), newMesh.emissiveSRV.Get()};

    D3DUtils::CreateVertexBuffer(device, meshData.vertices,
                                 newMesh.mVertexBuffer);
    D3DUtils::CreateIndexBuffer(device, meshData.indices, newMesh.mIndexBuffer);
    newMesh.indexCount = meshData.indices.size();
    mMeshes.push_back(newMesh);
    mMaterialData.metallic = 0.7f;
    mMaterialData.roughness = 0.3f;
    D3DUtils::CreateConstantBuffer(device, mMaterialData, mPixelCB);
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    const std::vector<ID3D11ShaderResourceView *> psNullSRVs(mResViews.size(),
                                                             nullptr);
    const std::vector<ID3D11ShaderResourceView *> vsNullSRVs(1, nullptr);

    for (int i = 0; i < mMeshes.size(); i++) {
        ID3D11ShaderResourceView *vsSRVs[1] = {mMeshes[i].heightSRV.Get()};

        context->IASetIndexBuffer(mMeshes[i].mIndexBuffer.Get(),
                                  DXGI_FORMAT_R32_UINT, 0);
        context->IASetVertexBuffers(0, 1,
                                    mMeshes[i].mVertexBuffer.GetAddressOf(),
                                    &mMeshes[i].stride, &mMeshes[i].offset);
        context->VSSetShaderResources(0, 1, vsSRVs);
        context->VSSetSamplers(0, mSamplerList.size(), mSamplerList.data());
        context->PSSetSamplers(0, mSamplerList.size(), mSamplerList.data());
        context->PSSetConstantBuffers(5, 1, mPixelCB.GetAddressOf());
        context->PSSetShaderResources(10, mResViews.size(), mResViews.data());
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->DrawIndexed(mMeshes[i].indexCount, 0, 0);

        // unbinding
        context->VSSetShaderResources(0, vsNullSRVs.size(), vsNullSRVs.data());
        context->PSSetShaderResources(10, psNullSRVs.size(), psNullSRVs.data());
    }
}