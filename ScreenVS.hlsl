struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 color : COLOR0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 posWorld : POSITION;
    float3 color : COLOR0;
    
};

PSInput main(VSInput vsInput)
{
    PSInput psInput;
    psInput.pos = float4(vsInput.pos.xyz, 1.0f);
    psInput.posWorld = vsInput.pos;
    psInput.normal = normalize(vsInput.normal);
    psInput.tex = vsInput.tex;
    psInput.color = vsInput.color;
    
    return psInput;
}