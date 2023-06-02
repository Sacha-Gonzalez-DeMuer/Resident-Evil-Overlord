//*************************//
// BASIC EFFECT [DEFERRED] //
//*************************//

/*
	- Diffuse Color/Texture
	- Specular Color
	- SpecularLevel Texture (Blinn)
	- Specular Intensity (Shininess)
	- NormalMap Texture
	- Ambient Color
	- Ambient Intensity [0-1]
	- Opacity Texture/Value [0-1]
*/

//GLOBAL MATRICES
//***************
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gViewInverse : VIEWINVERSE; // The ViewInverse Matrix - the third row contains the camera position!
float4x4 gWorld : WORLD;
float4x4 gWorldViewProj_Light;

float gShadowMapBias = 0.0005f;
float gLightIntensity = 1.0f;

Texture2D gShadowMap;

//STATES
//******
RasterizerState gRasterizerState
{
	FillMode = SOLID;
	CullMode = BACK;
};

BlendState gBlendState
{
	BlendEnable[0] = FALSE;
};

DepthStencilState gDepthState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
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
    AddressU = Wrap; // or Mirror or Clamp or Border
    AddressV = Wrap; // or Mirror or Clamp or Border
};

//LIGHT
//*****
float3 gLightDirection:DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool gUseDiffuseMap
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Bool";
> = false;

float4 gDiffuseColor
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float4(1, 1, 1, 1);

Texture2D gDiffuseMap
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
> ;

//SPECULAR
//********
float4 gSpecularColor
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float4(1, 1, 1, 1);

Texture2D gSpecularMap
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Texture";
> ;

bool gUseSpecularMap
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Bool";
> = false;

int gShininess <
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1f;
> = 55;

//AMBIENT
//*******
float4 gAmbientColor
<
	string UIName = "Ambient Color";
	string UIWidget = "Color";
> = float4(1.0, 1.0, 1.0, 1.0);

float gAmbientIntensity
<
	string UIName = "Ambient Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
> = 0.1f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel
<
	string UIName = "Flip Green Channel";
	string UIWidget = "Bool";
> = false;

bool gUseNormalMap
<
	string UIName = "Normal Mapping";
	string UIWidget = "Bool";
> = false;

Texture2D gNormalMap
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
> ;

//OPACITY
//***************
float gOpacityLevel <
	string UIName = "Opacity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
> = 1.0f;


//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 lPos : TEXCOORD1;
};

struct PS_Output
{
	float4 LightAccumulation : SV_TARGET0;
	float4 Diffuse : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
};

// The main vertex shader
VS_Output MainVS(VS_Input input) {

	VS_Output output = (VS_Output)0;

    output.lPos = mul(float4(input.Position, 1.0f), gWorldViewProj_Light);
	output.Position = mul(float4(input.Position, 1.0), gWorldViewProj);

	output.Normal = normalize(mul(input.Normal, (float3x3)gWorld));
	output.Tangent = normalize(mul(input.Tangent, (float3x3)gWorld));
	output.Binormal = normalize(mul(input.Binormal, (float3x3)gWorld));

	output.TexCoord = input.TexCoord;

	return output;
}

float2 texOffset(int u, int v)
{
    return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

float EvaluateShadowMap(float4 lpos)
{
	// re-homogize position after interpolation
    lpos.xyz /= lpos.w;

	// don't illuminate if position is not visible to the light 
    if (lpos.x < -1.0f || lpos.x > 1.0f ||
		lpos.y < -1.0f || lpos.y > 1.0f ||
		lpos.z < -1.0f || lpos.z > 1.0f)
        return 1;

	// clip space to texture space
    lpos.x = 0.5f * lpos.x + 0.5f;
    lpos.y = -0.5f * lpos.y + 0.5f;

	// apply shadow bias
    lpos.z -= gShadowMapBias;

	// sample shadow map
    float shadowMapDepth = gShadowMap.Sample(samPoint, lpos.xy).r;

	//perform PCF filtering 
    float x, y, sum;
    for (y = -3.5; y <= 3.5; y += 1.0)
        for (x = -3.5; x <= 3.5; x += 1.0)
            sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);


	//average the samples
    float shadowFactor = sum / 64.0f;
    return shadowFactor;
}

// The main pixel shader
PS_Output MainPS(VS_Output input){

	PS_Output output = (PS_Output)0;

	float shadowValue = EvaluateShadowMap(input.lPos);
	//Fill GBuffer
	
	//diffuse
	//*******
	float4 diffuse = gDiffuseColor;
	if(gUseDiffuseMap)
        diffuse *= gDiffuseMap.Sample(gTextureSampler, input.TexCoord);
    
    output.Diffuse = diffuse * shadowValue * gLightIntensity;
	
	
	//alpha
	//*****
    float alpha = diffuse.a * gOpacityLevel;
    clip(alpha - 0.1f); // if value <= 0 clip wil break out of the pixel shader
	
	//ambient
	//*******
    float4 ambient = gAmbientColor;
    ambient *= diffuse;
    ambient *= gAmbientIntensity;
	
	//set ambient
    output.LightAccumulation = ambient;
	
	//normal
	//******
    float3 normal = input.Normal;
	if(gUseNormalMap)
    {
        float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.Binormal),
		normalize(input.Normal));

        normal = gNormalMap.Sample(gTextureSampler, input.TexCoord).xyz;
        normal = normal * 2.0f - 1.0f;
        normal = mul(normal, TBN);
    }
	
	//set normal
    output.Normal = float4(normal, 0.0f);
	
	//specular
	//********
    float3 specular = gSpecularColor.rgb;
	if(gUseSpecularMap)
        specular *= gSpecularMap.Sample(gTextureSampler, input.TexCoord).rgb;

    float shininess = log2(gShininess) / 10.5f; // common way of rescaling shininess
	
	//set specular
    output.Specular = float4(specular, shininess);
	
	return output;
}

// Default Technique
technique10 Default {
	pass p0 {
		SetDepthStencilState(gDepthState, 0);
		SetRasterizerState(gRasterizerState);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}