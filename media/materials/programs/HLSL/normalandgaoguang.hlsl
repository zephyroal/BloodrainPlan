sampler2D texColor : register(s0);
sampler2D lightColor : register(s1);
sampler2D normalMap : register(s2);

float4x4 WorldViewProj;
float4x4 World;
float3 LightDiffuse;
float3 LightSpecular;
float3 LightDir;
float3 ambient;
float Shininess; 

float3 AmbientLight :	ambient_light_colour; 

struct VS_INPUT
{
	float4 Position : POSITION0;
	float3 Normal	: NORMAL0;
	float3 Tangent : TANGENT0;
	float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION0;
	float3 WorldPostion : TEXCOORD0;
	float3 WorldNormal	: TEXCOORD1;
	float3 WorldTangent : TEXCOORD2;
	float3 WorldBinormal : TEXCOORD3;
	float2 Texcoord : TEXCOORD4;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Position = mul(WorldViewProj, float4(In.Position.xyz, 1.0f));
	Out.WorldPostion = mul(World, float4(In.Position.xyz, 1.0f)).xyz;
	Out.Texcoord = In.Texcoord;
	Out.WorldNormal = mul(World, float4(In.Normal, 0.0f)).xyz;
	Out.WorldTangent = mul(World, float4(In.Tangent, 0.0f)).xyz;
	Out.WorldBinormal = cross(Out.WorldTangent, Out.WorldNormal);
	return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
	//构造切空间
	float3x3 TangentSpace;
	TangentSpace[0] = normalize(In.WorldTangent);
	TangentSpace[1] = normalize(In.WorldBinormal);
	TangentSpace[2] = normalize(In.WorldNormal);
	
	TangentSpace = transpose(TangentSpace);
	
	//提取normal
	float3 NormalMap = (tex2D(normalMap, In.Texcoord) - 0.5f) * 2.0f;
	NormalMap = mul(TangentSpace, NormalMap);
	NormalMap = normalize(NormalMap);
	
	//计算Diffuse
	LightDir = normalize(-LightDir);
	float3 Diffuse = LightDiffuse * saturate(dot(LightDir, NormalMap));
	//计算Specular
	float3 HalfDir = normalize(LightDir + NormalMap);
	float  SpecularCosine = saturate(dot(HalfDir, NormalMap));
	float3 Specular = LightSpecular * pow(SpecularCosine, Shininess);
	
	//计算高光效果
	float4 Color = tex2D(texColor, In.Texcoord);
  float3 texColor = Color.rgb;
  float specularFactor = tex2D(lightColor, In.Texcoord);
  float3 finalColour = Diffuse * texColor + specularFactor * Specular + ambient * texColor;
  return float4(finalColour, Color.a);
}