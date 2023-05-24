float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.3f;
float4x4 gBones[70];

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 BoneIndices : BLENDINDICES;
	float4 BoneWeights : BLENDWEIGHTS;
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

	// SKINNED VERTEX SHADER
	float4 transformedPosition;
	float3 transformedNormal;
	// iterate over all the bones
	for (int i = 0; i < 4; ++i)
	{
		// get the current boneindex
		int boneIndex = int(input.BoneIndices[i]);

		if (boneIndex < 0) continue;

		// calcualte bonespace position & normal
		float4 bonePos = mul(float4(input.pos, 1.0f), gBones[boneIndex]);
		float3 boneNormal = mul(input.normal, (float3x3)gBones[boneIndex]);

		// append this position & normal to the transformedPosition & transformedNormal based on the corresponding weight
		transformedPosition += bonePos * input.BoneWeights[i];
		transformedNormal += boneNormal * input.BoneWeights[i];
	}

	output.pos = mul(transformedPosition, gWorldViewProj);
	output.lPos = mul(float4(input.pos, 1.0f), gWorldViewProj_Light);
	output.normal = mul(transformedNormal, (float3x3)gWorld);
	output.texCoord = input.texCoord;

	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	float2 offset = float2(u, v);
	offset /= float2(1280, 720);
	return offset;
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

	//PCF sampling for shadow map
	float sum = 0;
	float x, y;

	
	//perform PCF filtering on a 4 x 4 texel neighborhood
	for (y = -1.5; y <= 1.5; y += 1.0)
		for (x = -1.5; x <= 1.5; x += 1.0)
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);

	//average the samples
	float shadowFactor = sum / 16.0f;


	//if clip space z value greater than shadow map value then pixel is in shadow
	//float shadowFactor = lpos.z > shadowMapDepth ? 1.0f : 0.0f;
 
	if (shadowFactor < lpos.z) return 0;
	return shadowFactor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb * shadowValue, color_a );
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

