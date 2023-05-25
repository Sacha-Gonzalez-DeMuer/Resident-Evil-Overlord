Texture2D<float4> inputTexture; // Input texture
SamplerState samplerState; // Sampler state

float4x4 gViewProjectionMatrix;
float3 gSampleOffsets[16]; // Sample offsets (precomputed)
float gOcclusionRadius; 
float gOcclusionBias;
float gOcclusionIntensity; 


struct VertexInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

PixelInput VSMain(VertexInput input)
{
    PixelInput output;
    output.position = input.position;
    output.texcoord = input.texcoord;
    return output;
}

float CalculateOcclusion(float3 position, float3 normal)
{
    float occlusion = 0.0;
    
    for (int i = 0; i < 16; i++)
    {
        float3 samplePos = position + gSampleOffsets[i] * gOcclusionRadius;
        float4 sampleProj = mul(float4(samplePos, 1.0), gViewProjectionMatrix);
        float2 sampleTexCoord = sampleProj.xy / sampleProj.w;
        
        // Retrieve depth from the input texture
        float sampleDepth = inputTexture.Sample(samplerState, sampleTexCoord).r;
        
        // Convert depth to view space
        float viewDepth = sampleDepth * gViewProjectionMatrix._33 + gViewProjectionMatrix._43;
        
        // Calculate occlusion factor
        float sampleOcclusion = gOcclusionBias - (position.z - viewDepth) / gOcclusionRadius;
        sampleOcclusion = saturate(sampleOcclusion);
        
        occlusion += sampleOcclusion;
    }
    
    return occlusion / 16.0;
}

float4 PSMain(PixelInput input) : SV_Target
{
    // Retrieve position and normal from the input texture
    float3 position = inputTexture.Sample(samplerState, input.texcoord).xyz;
    float3 normal = inputTexture.Sample(samplerState, input.texcoord).xyz * 2.0 - 1.0;
    
    // Calculate occlusion
    float occlusion = CalculateOcclusion(position, normal);
    
    // Apply occlusion to the final color
    float4 color = inputTexture.Sample(samplerState, input.texcoord);
    color.rgb *= gOcclusionIntensity * occlusion;
    
    return color;
}

technique AmbientOcclusion
{
    pass P0
    {
        VertexShader = compile vs_5_0 VSMain();
        PixelShader = compile ps_5_0 PSMain();
    }
}