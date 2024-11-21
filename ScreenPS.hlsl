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

float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    
    color *= 1.0f;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, float3(1.0, 1.0, 1.0) / 2.2);
    return color;
}

float3 lumaBasedReinhardToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / 2.2;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, invGamma);
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
    //return float4(Uncharted2ToneMapping(color.rgb), 1.0f);
    //return float4(LinearToneMapping(color.rgb), 1.0f);
    return float4(FilmicToneMapping(color.rgb), 1.0f);
    //return float4(lumaBasedReinhardToneMapping(color.rgb), 1.0f);
}