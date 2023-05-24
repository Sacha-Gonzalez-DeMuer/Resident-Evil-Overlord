//Deferred_DirectionalLightPas > Fullscreen Quad Render
#include "LightPass_Helpers.fx"

//VARIABLES
//*********
float4x4 gMatrixViewProjInv;
float3 gEyePos = float3(0, 0, 0);
Light gDirectionalLight;

//G-BUFFER DATA
//Texture2D gTextureAmbient; >> Already on Main RenderTarget
Texture2D gTextureDiffuse;
Texture2D gTextureSpecular;
Texture2D gTextureNormal;
Texture2D gTextureDepth;

SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

//VS & PS IO
//**********
struct VS_INPUT
{
	float3 Position: POSITION;
	float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//STATES
//******
RasterizerState gRasterizerState
{
	FillMode = SOLID;
	CullMode = BACK;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

BlendState gBlendState //Additive Blending (LIGHT-ACCUMULATION + LIGHTING-RESULTS)
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};

//VERTEX SHADER
//*************
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;

	return output;
}

//PIXEL SHADER
//************
float4 PS(VS_OUTPUT input) :SV_TARGET
{
	//Directional LightPass Logic
	//...
    int2 screenCoord = input.Position.xy;
    int3 loadCoord = int3(screenCoord, 0);
	
	//calculate pixel world pos for depth value
    float depth = gTextureDepth.Load(loadCoord).r;
    float3 pixelWorldPos = DepthToWorldPosition_QUAD(depth, input.TexCoord, gMatrixViewProjInv);
	
    float3 viewDir = normalize(pixelWorldPos - gEyePos);
	
	float3 diffuse = gTextureDiffuse.Load(loadCoord).rgb; 
	float4 specular = gTextureSpecular.Load(loadCoord);
    float shininess = exp2(specular.a * 10.5f);
	float3 normal = gTextureNormal.Load(loadCoord).xyz;
    float3 lightDir = normalize(gDirectionalLight.Direction.xyz);
	
    Material mat = (Material) 0;
    mat.Diffuse = diffuse;
    mat.Specular = specular;
    mat.Shininess = shininess;
	
	LightingResult result = DoDirectionalLighting(gDirectionalLight, mat, lightDir, viewDir, normal);
	
	
    return float4((mat.Diffuse * result.Diffuse) + (mat.Specular * result.Specular), 1.0f);
}

//TECHNIQUE
//*********
technique11 Default
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, VS() ));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
};
