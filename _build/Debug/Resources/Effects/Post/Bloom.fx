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



//PIXEL SHADER
//------------
float4 PS(PS_INPUT input) : SV_Target
{
    float4 baseColor = gTexture.Sample(samPoint, input.TexCoord);
    
    float dx = 1.0f / x;
    float dy = 1.0f / y;
    
    float2 dimensions = 0;
    gTexture.GetDimensions(dimensions.x, dimensions.y);
    
     // Apply a threshold to identify bright areas
    float threshold = 1.0;
    float4 brightAreas = (baseColor - threshold) / max(1.0 - threshold, 0.0001f);
    brightAreas = saturate(brightAreas);
    
       // Apply a blur to the bright areas
    float4 blurColor = float4(0, 0, 0, 0);
    float blurRadius = 1.f;
    
    int numPasses = 5;
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < numPasses; ++i)
    {
        for (int j = 0; j < numPasses; ++j)
        {
            float2 offset = float2(i * dx * 2.0f, j * dy * 2.0f);
            color += gTexture.Sample(samPoint, input.TexCoord + offset);
        }
    }


    color /= numPasses;

    return (color * brightAreas) + baseColor;
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