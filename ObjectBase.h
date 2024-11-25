#pragma once

// clang-format off
/*****************************************************************//**
 * \file   GObject.h
 * \brief  declare base class for renderable classes
 * 
 * \author Hoyeon
 * \date   August 2024
 *********************************************************************/
// clang-format on

#include "Mesh.h"
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <wrl/client.h>

enum OjbectType { DEFAULT_OBJECT, SKYBOX, PBR_MODEL };

using namespace Microsoft::WRL;

class ObjectBase {
  public:
    ObjectBase() { std::cout << "OBJBase open\n"; };

    virtual ~ObjectBase() { std::cout << "ObjectBase closed\n"; };

    virtual void Initialize(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context,
                            std::vector<MeshData> &meshes) = 0;

    virtual void Initialize(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context,
                            MeshData &meshes) = 0;
    virtual void
    UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) = 0;

    virtual void Render(ComPtr<ID3D11DeviceContext> &context) = 0;
};