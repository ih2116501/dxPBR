#pragma once
#include <directxtk/SimpleMath.h>

struct ConstData {
    DirectX::SimpleMath::Matrix proj;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix vp;
    DirectX::SimpleMath::Vector3 eyePos;
    float dummy;
    DirectX::SimpleMath::Matrix invTranspose;
};