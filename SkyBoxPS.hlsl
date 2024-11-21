#include "Common.hlsli"

TextureCube envIBLTex : register(t0);
TextureCube specularIBLTex : register(t1);
TextureCube irradianceIBLTex : register(t2);
Texture2D brdfTex : register(t3);

struct SBPSInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 posWorld : POSITION;
    float4 color : COLOR0;
};


float4 main(SBPSInput input) : SV_TARGET
{
    float3 env = envIBLTex.Sample(g_sampler, input.posWorld);
    return float4(env, 1.0f);
    //return float4(FilmicToneMapping(env), 1.0f);

    //return envIBLTex.Sample(g_sampler, input.posWorld);
    //return float4(0.8f, 0.2f, 0.6f, 1.0f);
    //return input.color;
}