#include "Common.hlsli"

TextureCube envIBLTex : register(t0);
TextureCube specularIBLTex : register(t1);
TextureCube irradianceIBLTex : register(t2);
Texture2D brdfTex : register(t3);

Texture2D albedoTex : register(t10);
Texture2D normalTex : register(t11);
Texture2D aoTex : register(t12);
Texture2D metallicTex : register(t13);
Texture2D roughnessTex : register(t14);
Texture2D emissiveTex : register(t15);


cbuffer Material : register(b5)
{
    float cMetallic;
    float cRoughness;
    float2 dummyy;
}

float3 SchlickFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * VdotH - 6.98316) * VdotH);
    //return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                   float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.Sample(clampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness)).rg;
    float3 specularIrradiance = specularIBLTex.SampleLevel(g_sampler, reflect(-pixelToEye, normalWorld),
                                                            roughness * 5).rgb;

    const float3 Fdielectric = 0.04;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                  float metallic, float3 H)
{
    float3 Fdielectric = 0.04f;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(H, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance = irradianceIBLTex.SampleLevel(g_sampler, normalWorld, 0).rgb;
    return kd * albedo * irradiance;
    //return irradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
                            float metallic, float roughness, float3 H)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic, H);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    //return diffuseIBL;

    return (diffuseIBL + specularIBL) * ao;
}

float4 main(PSInput psInput) : SV_TARGET
{
    float3 tNormal = normalTex.SampleLevel(g_sampler, psInput.tex, 0).rgb;
    tNormal = 2.0 * tNormal - float3(1.0, 1.0, 1.0); // [-1.0, 1.0]
    float3 N = psInput.normal;
    float3 T = normalize(psInput.tangent - dot(psInput.tangent, N) * N);
    float3 B = cross(N, T);
        
    float3x3 TBN = float3x3(T, B, N);
    float3 normalWorld = normalize(mul(tNormal, TBN));
    //float3 normalWorld = psInput.normal;

    float3 basicColor = float3(0.0f, 0.0f, 0.0f);
    float3 toLight = normalize(lightPos - psInput.posWorld);
    float3 toEye = normalize(psInput.eyePos - psInput.posWorld);
    float3 H = normalize(toEye + toLight);
    float ndoth = dot(H, psInput.normal);
    
    // IBL shader
    float3 albedo = albedoTex.SampleLevel(g_sampler, psInput.tex, 0).rgb;
    //return float4(albedo, 1.0f);
    float tmetallic = metallicTex.SampleLevel(g_sampler, psInput.tex, 0).r;
    float tRoughness = roughnessTex.SampleLevel(g_sampler, psInput.tex, 0).r;
    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, toEye, 1.0f, tmetallic, tRoughness, H);
    
    //return float4(specularIBLTex.SampleLevel(g_sampler, reflect(-toEye, psInput.normal),
    //                                                        1.5).rgb, 1.0f);
    return float4(ambientLighting, 1.0);
}