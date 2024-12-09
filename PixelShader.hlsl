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
Texture2D metallicRoughnessTex : register(t16);


cbuffer PixelConstants : register(b5)
{
    int useWireframe = 0;
    int useMetallicRoughnessTex = 0;
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
                  float metallic)
{
    
    float3 Fdielectric = 0.04f;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance = irradianceIBLTex.SampleLevel(g_sampler, normalWorld, 0).rgb;
    return kd * albedo * irradiance;
    //return irradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
                            float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    return (diffuseIBL + specularIBL) * ao;
}

float4 main(PSInput psInput) : SV_TARGET
{
    if (useWireframe)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        float3 tNormal = normalTex.SampleLevel(g_sampler, psInput.tex, 0).rgb;
        //tNormal.y = -tNormal.y;
        tNormal = 2.0 * tNormal - float3(1.0, 1.0, 1.0); // [-1.0, 1.0]
        
        float3 N = psInput.normal;
        float3 T = normalize(psInput.tangent - dot(psInput.tangent, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        float3 normalWorld = normalize(mul(tNormal, TBN));

        float3 toEye = normalize(psInput.eyePos - psInput.posWorld);
    
        float3 albedo = albedoTex.SampleLevel(g_sampler, psInput.tex, 0).rgb;
        //return float4(albedo, 1.0f);
        float tMetallic = 0;
        float tRoughness = 0;
        
        if (useMetallicRoughnessTex == 0)
        {
            tMetallic = metallicTex.SampleLevel(g_sampler, psInput.tex, 0).r;
            tRoughness = roughnessTex.SampleLevel(g_sampler, psInput.tex, 0).r;
        }
        else
        {
            tMetallic = metallicRoughnessTex.SampleLevel(g_sampler, psInput.tex, 0).b;
            tRoughness = metallicRoughnessTex.SampleLevel(g_sampler, psInput.tex, 0).g;
        }
        float ao = aoTex.Sample(g_sampler, psInput.tex).r;
        float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, toEye, ao, tMetallic, tRoughness);
        float3 emissive = emissiveTex.Sample(g_sampler, psInput.tex);
        return float4(ambientLighting + emissive*2.0, 1.0);
    }
}