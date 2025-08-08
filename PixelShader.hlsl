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

struct Light
{
    float3 pos;
    float dummy;
    float3 color;
    float dummy1;
};

cbuffer PixelConstants : register(b5)
{
    int useWireframe = 0;
    int useMetallicRoughnessTex = 0;
    int option1;
    float roughIntensity;
    Light light;
}

float3 SchlickFresnel(float3 F0, float NdotH)
{
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
    //return F0 + (1.0 - F0) * pow(1.0 - NdotH, 5.0);
}

float3 SchlickFresnelRoughness(float3 F0, float NdotH, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
}


float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                   float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.Sample(clampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness)).rg;
    float3 specularIrradiance = specularIBLTex.SampleLevel(g_sampler, reflect(-pixelToEye, normalWorld), roughness * roughIntensity).rgb;

    const float3 Fdielectric = 0.04;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnelRoughness(F0, max(0.0f, dot(normalWorld, pixelToEye)), roughness);
    return (F * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                  float metallic)
{
    
    float3 Fdielectric = 0.04f;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance = irradianceIBLTex.Sample(g_sampler, normalWorld).rgb;
    return kd * albedo * irradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normal, float3 pixelToEye, float ao,
                            float metallic, float roughness)
{
    float3 F0 = 0.04f;
    //float3 F = SchlickFresnel(F0, max(0.0, dot(normal, pixelToEye)));
    //float3 F2 = SchlickFresnelRoughness(F0, max(0.0, dot(normal, pixelToEye)), roughness);
    //float3 c = float3(1.0f, 1.0f, 1.0f);
    //if (option1 == 0)
    //{
    //return F * c;
    //}
    //else
    //{
    //    return F2 * c;
    //}
    float3 diffuseIBL = DiffuseIBL(albedo, normal, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normal, pixelToEye, metallic, roughness);
    return (diffuseIBL + specularIBL) * ao;
}

float3 blinn(float3 color, float3 normalWorld, float3 toLight, float3 toEye)
{
    float3 halfway = normalize(toLight + toEye);
    return color * pow(max(dot(normalWorld, halfway), 0.0), 5.0f);
}

float DistributionGGX(float3 normalWorld, float3 toLight, float3 toEye, float roughness)
{
    float PI = 3.14159265359;
    float a = roughness * roughness;
    float a2 = a * a;
    float3 halfway = normalize(toLight + toEye);
    float NdotH = max(dot(normalWorld, halfway), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float ndotv, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float denom = ndotv * (1.0 - k) + k;
    return ndotv / denom;
}

float GeometrySmith(float3 normalWorld, float3 toLight, float3 toEye, float roughness)
{
    float NdotV = max(dot(normalWorld, toEye), 0.0);
    float NdotL = max(dot(normalWorld, toLight), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 CaculatePointLight(float3 albedo, float metallic, float3 normalWorld, float3 toEye, float3 v, float roughness)
{
    

    const float3 Fdielectric = 0.04;
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnelRoughness(F0, max(0.0f, dot(normalWorld, toEye)), roughness);
        
    float3 toLight = normalize(light.pos);
    float distance = length(v);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = light.color * attenuation;

        
    float NDF = DistributionGGX(normalWorld, toLight, toEye, roughness);
    float G = GeometrySmith(normalWorld, toLight, toEye, roughness);
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normalWorld, toEye), 0.0) * max(dot(normalWorld, toLight), 0.0) + 0.0001;
    float3 specular = numerator / denominator;
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
  
    kD *= 1.0 - metallic;
    float PI = 3.14159265359;
  
    float NdotL = max(dot(normalWorld, toLight), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
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
        tNormal = 2.0 * tNormal - float3(1.0, 1.0, 1.0); // [-1.0, 1.0]
        
        float3 N = psInput.normal;
        float3 T = normalize(psInput.tangent - dot(psInput.tangent, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        float3 normalWorld = normalize(mul(tNormal, TBN));

        float3 toEye = normalize(psInput.eyePos - psInput.posWorld);
        float3 albedo = albedoTex.SampleLevel(g_sampler, psInput.tex, 0).rgb;
        float tMetallic = 0;
        float tRoughness = 0;
        
        if (useMetallicRoughnessTex == 0)
        {
            tMetallic = metallicTex.SampleLevel(g_sampler, psInput.tex, 0).r;
            tRoughness = roughnessTex.SampleLevel(g_sampler, psInput.tex, 0).r;
        }
        else
        {
            tMetallic = metallicTex.SampleLevel(g_sampler, psInput.tex, 0).b;
            tRoughness = metallicTex.SampleLevel(g_sampler, psInput.tex, 0).g;
        }
        
        float ao = aoTex.Sample(g_sampler, psInput.tex).r;
        float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, toEye, ao, tMetallic, tRoughness);
        float3 emissive = emissiveTex.Sample(g_sampler, psInput.tex);

        float3 Lo = CaculatePointLight(albedo, tMetallic, normalWorld, toEye, light.pos-psInput.posWorld, tRoughness);
        //return float4(Lo , 1.0);
        return float4(ambientLighting + Lo + emissive * 2.0, 1.0);
        //return float4(ambientLighting * blinn(light.color, normalWorld, (light.pos - psInput.posWorld), toEye) + emissive * 2.0, 1.0);
        //return float4(blinn(lightColor, normalWorld, (lightPos - psInput.posWorld), toEye), 1.0);
        //return float4(ambientLighting + emissive * 2.0, 1.0);
        
    }
}