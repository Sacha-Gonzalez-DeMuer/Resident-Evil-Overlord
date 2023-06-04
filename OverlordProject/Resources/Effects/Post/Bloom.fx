//=============================================================================
//// Shader uses position and texture
//=============================================================================


SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState BackCulling
{
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Position = float4(input.Position, 1.0f);
    output.TexCoord = input.TexCoord;
    return output;
}

float gThreshold = .5f;
float3 gGrayScales = float3(0.1f, 0.5f, 0.114f);
float4 gBlurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
float gBlurRadius = 1.0f;
float gSmoothness = 0.1f;
float gIntensity = .3f;

// PIXEL SHADER
// ------------
float4 PS(PS_INPUT input) : SV_Target
{
    float4 baseColor = gTexture.Sample(samPoint, input.TexCoord);

    // Convert the base color to grayscale
    float grayscale = (baseColor.r + baseColor.g + baseColor.b) / 3.0f;

    // Apply a threshold to identify bright areas
    float brightAreas = saturate((grayscale - gThreshold) / (1.0f - gThreshold));

    // Apply a blur to the bright areas
    float4 blurColor = gBlurColor;
    int numSamples = 0;
    float blurRadius = gBlurRadius;
    float sampleStep = blurRadius * 0.5f;

    float2 dimensions;
    gTexture.GetDimensions(dimensions.x, dimensions.y);
    float2 texelSize = float2(3.0 / dimensions.x, 3.0 / dimensions.y);

    for (float x = -blurRadius; x <= blurRadius; x += sampleStep)
    {
        for (float y = -blurRadius; y <= blurRadius; y += sampleStep)
        {
            float2 offset = float2(x * texelSize.x, y * texelSize.y);
            blurColor += gTexture.Sample(samPoint, input.TexCoord + offset);
            numSamples++;
        }
    }

    blurColor /= numSamples;

    return blurColor * brightAreas * gIntensity + baseColor;
}


//TECHNIQUE
//---------
technique11 Bloom
{
    pass P0
    {
		// Set states...
        SetDepthStencilState(EnableDepth, 0);
        SetRasterizerState(BackCulling);
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}