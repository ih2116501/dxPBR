#pragma once
#include<directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

struct Light{
    Vector3 lightPos = Vector3(0.0f, 0.0f, -2.0f);
    float radiance = 0.5f;
    Vector3 dir = Vector3(0.0f);
    float fallOffStart = 0.0f;
    Vector3 color = Vector3(1.0f);
    float fallOffEnd = 20.0f;
    Vector3 dummy;
    float pow = 10.0f;

};
