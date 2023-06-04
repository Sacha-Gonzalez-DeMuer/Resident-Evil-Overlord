float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.005f;
float gLightIntensity = 1.0f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;
Texture2D gNormalMap;
bool gUseNormalMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerComparisonState cmpSampler
{
	// sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;

	// sampler comparison state
    ComparisonFunc = LESS_EQUAL;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	//TODO: complete Vertex Shader
	output.lPos = mul(float4(input.pos, 1.0f), gWorldViewProj_Light);
	output.pos = mul(float4(input.pos, 1.0f), gWorldViewProj);
	output.normal = input.normal;
    output.texCoord = input.texCoord;
	
	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

float EvaluateShadowMap(float4 lpos)
{
	// re-homogize position after interpolation
	lpos.xyz /= lpos.w;

	// don't illuminate if position is not visible to the light 
	if (lpos.x < -1.0f || lpos.x > 1.0f ||
		lpos.y < -1.0f || lpos.y > 1.0f ||
		lpos.z < -1.0f || lpos.z > 1.0f) return 1;

	// clip space to texture space
	lpos.x = 0.5f * lpos.x + 0.5f;
	lpos.y = -0.5f * lpos.y + 0.5f;

	// apply shadow bias
	lpos.z -= gShadowMapBias;

	//perform PCF filtering 
    float x, y, sum;
    for (y = -3.5; y <= 3.5; y += 1.0)
        for (x = -3.5; x <= 3.5; x += 1.0)
            sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);


	//average the samples
    float shadowFactor = sum / 64.0f;
    float ambient = .1f;
    return shadowFactor + ambient;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float shadowValue = EvaluateShadowMap(input.lPos);

    float4 diffuseColor = gDiffuseMap.Sample(samLinear, input.texCoord);
    float3 color_rgb = diffuseColor.rgb;
    float color_a = diffuseColor.a;
    clip(color_a - 0.65f);
	
    // Diffuse calculation
    float diffuseStrength = dot(input.normal, -gLightDirection);
    diffuseStrength = diffuseStrength * 0.5 + 0.5;
    diffuseStrength = saturate(diffuseStrength);
    color_rgb = color_rgb * diffuseStrength;

    // Normal mapping
    float3 normalMap = gUseNormalMap ? gNormalMap.Sample(samLinear, input.texCoord).rgb * 2 - 1 : input.normal;
    normalMap = normalize(mul(float4(normalMap, 0), (float4x4) gWorld));

    // Calculate light intensity based on normal map
    float normalIntensity = saturate(dot(normalMap, -gLightDirection));
    color_rgb *= normalIntensity;

    return float4(color_rgb * shadowValue * gLightIntensity, color_a);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

