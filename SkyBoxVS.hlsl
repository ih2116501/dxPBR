#include "Common.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 color : COLOR0;
    float3 tangent : TANGENT0;
};

struct SBPSInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 posWorld : POSITION;
    float4 color : COLOR0;
};

SBPSInput main(VSInput vsInput)
{
    SBPSInput vsOut;
    vsOut.posWorld = vsInput.pos;
    vsOut.pos = float4(vsInput.pos.xyz, 1.0f);
    float4x4 viewProj = mul(view, proj);
    vsOut.pos = mul(vsOut.pos, viewProj);

    vsOut.normal = vsInput.normal;
    vsOut.color = float4(vsInput.color, 1.0f);
	return vsOut;
}