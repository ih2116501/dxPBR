SamplerState g_sampler : register(s0);
SamplerState clampSampler : register(s1);

cbuffer VSConstant : register(b0)
{
    float4x4 proj;
    float4x4 view;
    float4x4 model;
    float4x4 vp;
    float3 eyePos;
    float dummy;
    float4x4 invTranspose;
};

cbuffer Light : register(b1)
{
    float3 lightPos;
    float radiance;
    float3 lightDir;
    float fallOffStart;
    float3 color;
    float fallOffEnd;
    float3 dummy2;
    float lightPow;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 posWorld : POSITION0;
    float3 color : COLOR0;
    float3 eyePos : POSITION1;
    float3 tangent : TANGENT0;
};
