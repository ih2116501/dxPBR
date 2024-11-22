#include "Common.hlsli"

Texture2D heightTexture : register(t0);

struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 color : COLOR0;
    float3 tangent : TANGENT0;
};

PSInput main(VSInput vsInput)
{
    PSInput psInput;
    psInput.normal = normalize(vsInput.normal);

    float tHeight = heightTexture.SampleLevel(g_sampler, vsInput.tex, 0).r;
    tHeight = tHeight * 2.0 - 1.0;
    float3 posOut = vsInput.pos;
    
    posOut += vsInput.normal * tHeight * 0.2f;
    psInput.pos = float4(vsInput.pos.xyz, 1.0f);
    psInput.pos = float4(posOut, 1.0f);

    float4x4 viewProj = mul(view, proj);
    psInput.posWorld = posOut;

    psInput.pos = mul(psInput.pos, viewProj);
    psInput.tex = vsInput.tex;
    psInput.color = vsInput.color;
    psInput.eyePos = eyePos;
    psInput.tangent = vsInput.tangent;

    return psInput;
}