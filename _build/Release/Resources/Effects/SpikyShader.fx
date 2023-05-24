//************
// VARIABLES *
//************
float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
float4x4 m_MatrixWorld : WORLD;
float3 m_LightDirection : DIRECTION
<
	string Object="TargetLight";
> = float3(0.577f, -0.577f, 0.577f);

float3 gColorDiffuse : COLOR = float3(1,1,1);

float gSpikeLength
<
	string UIWidget="Slider";
	float UIMin=0.0f;
	float UIMax=0.5f;
	float UIStep=0.0001f;
> = 0.1f;

RasterizerState FrontCulling 
{ 
	CullMode = FRONT; 
};

//**********
// STRUCTS *
//**********
struct VS_DATA
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
	//Step 1.
	//Delete this transformation code and just return the VS_DATA parameter (vsData)
	//Don't forget to change the return type!

    return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float3 normal, float2 texCoord)
{
	//Step 1. Create a GS_DATA object
    GS_DATA temp = (GS_DATA) 0;

	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4)
    temp.Position = mul(float4(pos, 1.0f), m_MatrixWorldViewProj);
    
	//Step 3. Transform the normal using the World Matrix and assign it to (GS_DATA object).Normal (Only Rotation, No translation!)
    temp.Normal = mul(normal, (float3x3) m_MatrixWorld);

	//Step 4. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
    triStream.Append(temp);
}

[maxvertexcount(9)]
void SpikeGenerator(triangle VS_DATA vertices[3], inout TriangleStream<GS_DATA> triStream)
{
	//Step 1. Calculate CENTER_POINT
    float3 CENTER_POINT = (vertices[0].Position + vertices[1].Position + vertices[2].Position) / 3.0f;
    
	//Step 2. Calculate Face Normal (Original Triangle)
    float3 spikeNormal = normalize((vertices[0].Normal + vertices[1].Normal + vertices[2].Normal) / 3.0f);
    
	//Step 3. Offset CENTER_POINT (use gSpikeLength)
    CENTER_POINT += spikeNormal * gSpikeLength;

	//Step 4 + 5. Calculate Individual Face Normals (Cross Product of Face Edges) & Create Vertices for every face
    //FACE 1
    float3 faceNormal1 = normalize(cross(vertices[1].Position - vertices[0].Position, vertices[0].Position - CENTER_POINT));
    
    CreateVertex(triStream, CENTER_POINT, faceNormal1, float2(0, 1));
    CreateVertex(triStream, vertices[0].Position, faceNormal1, float2(0, 0));
    CreateVertex(triStream, vertices[1].Position, faceNormal1, float2(1, 0));

    //Restart Strip! >> We want to start a new triangle (= interrupt trianglestrip)
    triStream.RestartStrip();
    
    //FACE 2
    float3 faceNormal2 = normalize(cross(vertices[2].Position - vertices[1].Position, vertices[1].Position - CENTER_POINT));
   
    CreateVertex(triStream, CENTER_POINT, faceNormal2, float2(0, 1));
    CreateVertex(triStream, vertices[1].Position, faceNormal2, float2(0, 0));
    CreateVertex(triStream, vertices[2].Position, faceNormal2, float2(1, 0));
    
    triStream.RestartStrip();
    
    //Face 3
    float3 faceNormal3 = normalize(cross(vertices[0].Position - vertices[2].Position, vertices[2].Position - CENTER_POINT));
    CreateVertex(triStream, CENTER_POINT, faceNormal3, float2(0, 1));
    CreateVertex(triStream, vertices[2].Position, faceNormal3, float2(1, 0));
    CreateVertex(triStream, vertices[0].Position, faceNormal3, float2(0, 0));
    
    triStream.RestartStrip();
}


//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    input.Normal=normalize(input.Normal);
	float diffuseStrength = max(dot(normalize(m_LightDirection),-input.Normal),0.2f); 
	return float4(gColorDiffuse*diffuseStrength,1.0f);
}

//*************
// TECHNIQUES *
//*************
technique10 Default 
{
    pass p0
    {
        SetRasterizerState(FrontCulling);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}