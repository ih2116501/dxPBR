#include "Common.hlsli"

Texture2D screenTex : register(t0);

struct PSIn
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 posWorld : POSITION;
    float3 color : COLOR0;
    
};

float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / 2.2f;

    color = pow(color, invGamma);
    color = clamp(1.0 * color, 0., 1.);
    return color;
}

float3 FilmicToneMapping(float3 color)
{
    color = max(float3(0, 0, 0), color);
    color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

float4 main(PSIn psInput) : SV_TARGET
{
    float4 color = screenTex.Sample(g_sampler, psInput.tex);
    return float4(FilmicToneMapping(color.rgb), 1.0f);
}