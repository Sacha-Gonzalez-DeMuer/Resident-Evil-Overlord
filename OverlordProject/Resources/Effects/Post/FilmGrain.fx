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

float gGrainAmount = .1f;
float gGrainSize = .3f;

float GetNoise(float2 uv) /* https://gist.github.com/keijiro/ee7bc388272548396870 */
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}


// PIXEL SHADER
// ------------
float4 PS(PS_INPUT input) : SV_Target
{
    // Sample the texture color
    float4 baseColor = gTexture.Sample(samPoint, input.TexCoord);

    // Generate film grain noise
    float grainAmount = gGrainAmount; // Adjust this parameter to control the amount of grain
    float grainSize = gGrainSize; // Adjust this parameter to control the size of the grain
    float noise = GetNoise(input.TexCoord) * grainAmount;

    // Apply film grain
    float4 filmGrain = float4(noise, noise, noise, 0.0f);

    return baseColor - filmGrain ;
}
 

//TECHNIQUE
//---------
technique11 Grain
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