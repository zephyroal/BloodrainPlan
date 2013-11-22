
/*
    ZERO = 1,
    ONE = 2,
    SRCCOLOR = 3,
    INVSRCCOLOR = 4,
    SRCALPHA = 5,
    INVSRCALPHA = 6,
    DESTALPHA = 7,
    NVDESTALPHA = 8,
    DESTCOLOR = 9,
    INVDESTCOLOR = 10
*/
bool g_AlphaBlendEnable
<
	string UIName = "Alpha Blend";
> = false;

int g_BlendOP
<
	string UIName = "Blend OP";
	int UIMin=0;
  int UIMax=4;
  int UIStep=1;
> = 0;

int g_SrcBlend
<
	string UIName = "Src Blend";
	int UIMin=1;
  int UIMax=10;
  int UIStep=1;
> = 5;

int g_DestBlend
<
	string UIName = "Dest Blend";
	int UIMin=1;
  int UIMax=10;
  int UIStep=1;
> = 6;

/*
    D3DCMP_NEVER = 1,
    D3DCMP_LESS = 2,
    D3DCMP_EQUAL = 3,
    D3DCMP_LESSEQUAL = 4,
    D3DCMP_GREATER = 5,
    D3DCMP_NOTEQUAL = 6,
    D3DCMP_GREATEREQUAL = 7,
    D3DCMP_ALWAYS = 8,
*/

bool g_AlphaTestEnable
<
	string UIName = "Alpha Test";
> = false;

int g_AlphaRef
<
	string UIName = "Alpha Test Value";
	int UIMin=1;
  int UIMax=255;
  int UIStep=1;
> = 20;

int g_AlphaFunc
<
	string UIName = "Alpha Test Function";
	int UIMin=1;
  int UIMax=8;
  int UIStep=1;
> = 5;

int g_CullMode
<
	string UIName = "Cull Mode";
	int UIMin=1;
  int UIMax=3;
  int UIStep=1;
> = 2;

int g_ZFunc
<
	string UIName = "Z Function";
	int UIMin=1;
  int UIMax=8;
  int UIStep=1;
> = 4;

bool g_DepthBiasEnable
<
	string UIName = "Depth Bias Enable";
> = false;

bool g_ZEnable
<
	string UIName = "Z Test";
> = true;

bool g_ZWriteEnable
<
	string UIName = "Z Write";
> = true;

bool g_ColorWriteEnable
<
	string UIName = "Color Write";
> = true;

//for debug start
bool g_DisplayDebugColor <
    string UIName = "Display Debug Color";
> = false;
float g_DisplayDiffuse1 <
    string UIName = "Display Diffuse1";
> = 0.0f;
float g_DisplayDiffuse2 <
    string UIName = "Display Diffuse2";
> = 0.0f;
float g_DisplayNormal <
    string UIName = "Display Normal";
> = 0.0f;
float g_DisplayHeight <
    string UIName = "Display Height";
> = 0.0f;
float g_DisplayGlossness <
    string UIName = "Display Glossness";
> = 0.0f;
//for debug end

bool g_BindToCamera <
    string UIName = "Bind To Camera";
> = false;

bool g_UseVertexColor <
	string UIName = "Use Vertex Color";
> =false;

bool g_Diffuse1Enable <
	string UIName = "Diffuse 1 Enable";
> = false;

bool g_Diffuse2UseSelfUV <
	string UIName = "Diffuse 2 Use Self UV";
> =false;

bool g_Diffuse2Add <
	string UIName = "Diffuse 2 Add";
> =false;

bool g_Diffuse2Mul <
	string UIName = "Diffuse 2 Mul";
> =false;

bool g_Diffuse2Enable <
	string UIName = "Diffuse 2 Enable";
> = false;

texture g_Diffuse2Texture < 
	string UIName = "Diffuse 2 Map";
	int Texcoord = 1;
	int MapChannel = 2;
>;

bool g_GlossnessTextureEnable <
	string UIName = "Glossness Texture Enable";
> = false;

bool g_NormalEnable <
	string UIName = "Normal Enable";
> = false;

bool g_NormalUseSelfUV <
	string UIName = "Normal Use Self UV";
> = false;

texture g_NormalTexture < 
	string UIName = "Normal";
	//int Texcoord = 0;
	//int MapChannel = 1;    
>;

bool g_ParallaxEnable <
	string UIName = "Normal Map Parallax";
> =false;


float g_ParallaxScale <
	string UIName = "Parallax Scale";
	float UIMin = -0.50f;
	float UIMax = 0.5f;
	float UIStep = 0.01f;
>   = 0.02f;

float g_ParallaxBias <
	string UIName = "Parallax Bias";
	float UIMin = -0.5f;
	float UIMax = 0.5f;
	float UIStep = 0.01f;
>   = 0.0f;


texture g_Diffuse1Texture : DiffuseMap< 
	string UIName = "Diffuse 1 Map ";
	int Texcoord = 0;
	int MapChannel = 1;
>;

//WRAP = 1,MIRROR = 2,CLAMP = 3,BORDER = 4
int g_Diffuse1_AddressU
<
	string UIName = "Diffuse 1 Address U";
	int UIMin=1;
  int UIMax=4;
  int UIStep=1;
> = 1;

int g_Diffuse1_AddressV
<
	string UIName = "Diffuse 1 Address V";
	int UIMin=1;
  int UIMax=4;
  int UIStep=1;
> = 1;

sampler2D DiffuseSampler = sampler_state
{
	Texture = <g_Diffuse1Texture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
  AddressU = <g_Diffuse1_AddressU>;
  AddressV = <g_Diffuse1_AddressV>;
  AddressW = WRAP;
};

//Billboard
bool g_BillboardEnable <
	string UIName = "Billboard Enable";
> = false;

float g_BillboardSize <
	string UIName = "Billboard Size";
	float UIMin = 0.001f;
	float UIMax = 60.0f;
	float UIStep = 0.1f;
> = 1.0f;

float g_BillboardDepthOffset <
	string UIName = "Billboard Depth Offset";
	float UIMin = -20.0f;
	float UIMax = 20.0f;
	float UIStep = 0.01f;
> = 0.0f;


//texture frame animate
bool g_Texture1LayerAnimationEnable <
	string UIName = "Texture 1 Layer Animation Enable";
> = false;

float g_Texture1LayerAnimationSpeed <
	string UIName = "Texture 1 Layer Animation Speed";
	float UIMin = 0.001f;
	float UIMax = 60.0f;
	float UIStep = 0.01f;
> = 0.5f;

//texture frame animate
bool g_Texture1AnimationEnable <
	string UIName = "Texture 1 Animation Enable";
> = false;

float g_Texture1AnimationFrame <
	string UIName = "Texture 1 Frame Per Second";
	float UIMin = 0.001f;
	float UIMax = 60.0f;
	float UIStep = 0.1f;
> = 5.0f;

float g_Texture1FrameCountH <
	string UIName = "Texture 1 Frame Count H";
	float UIMin = 1;
	float UIMax = 16;
	float UIStep = 1;
> = 2;

float g_Texture1FrameCountV <
	string UIName = "Texture 1 Frame Count V";
	float UIMin = 1;
	float UIMax = 16;
	float UIStep = 1;
> = 2;

float g_Texture1FrameCountH_inv <
	string UIName = "Texture 1 Frame Count H INV";
	float UIMin = 0.0f;
	float UIMax = 1.0f;
	float UIStep = 0.0001;
> = 0.5f;

float g_Texture1FrameCountV_inv <
	string UIName = "Texture 1 Frame Count V INV";
	float UIMin = 0.0f;
	float UIMax = 1.0f;
	float UIStep = 0.0001;
> = 0.5f;


//texture uv animate

float g_UV1TimeLength
<
	string UIName = "UV 1 Time Length";
	float UIMin = 0.1f;
	float UIMax = 1000.0f;
	float UIStep = 0.1f;
>   = 10000.0f;

#define g_UV1Time fmod(time,g_UV1TimeLength)


bool g_UV1ScrollEnable <
	string UIName = "UV 1 Scroll Enable";
> = false;

float g_U1Scroll
<
	string UIName = "UV 1 Scroll";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;    
>   = 0.0f;

float g_V1Scroll
<
	string UIName = "UV 1 Scroll";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;    
>   = 0.0f;

bool g_UV1RotateEnable <
	string UIName = "UV 1 Rotate Enable";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;    
> = false;

float g_UV1Rotate
<
	string UIName = "UV 1 Rotate";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;    
>   = 0.0f;


//texture uv animate

float g_UV2TimeLength
<
	string UIName = "UV 2 Time Length";
	float UIMin = 0.1f;
	float UIMax = 1000.0f;
	float UIStep = 0.1f;
>   = 10000.0f;
#define g_UV2Time fmod(time,g_UV2TimeLength)

bool g_UV2ScrollEnable <
	string UIName = "UV 2 Scroll Enable";
> = false;

float g_U2Scroll
<
	string UIName = "UV 2 Scroll";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;
>   = 0.0f;

float g_V2Scroll
<
	string UIName = "UV 2 Scroll";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;
>   = 0.0f;

bool g_UV2RotateEnable <
	string UIName = "UV 2 Rotate Enable";
> = false;

float g_UV2Rotate
<
	string UIName = "UV 2 Rotate";
	float UIMin = -10.0f;
	float UIMax = 10.0f;
	float UIStep = 0.1f;    
>   = 0.0f;

bool g_UV2AlphaNoAnimation <
	string UIName = "UV 2 Alpah No Animation";
> = false;

int g_Diffuse2_AddressU
<
	string UIName = "Diffuse 2 Address U";
	int UIMin=1;
  int UIMax=4;
  int UIStep=1;
> = 1;

int g_Diffuse2_AddressV
<
	string UIName = "Diffuse 2 Address V";
	int UIMin=1;
  int UIMax=4;
  int UIStep=1;
> = 1;

sampler2D Diffuse2Sampler = sampler_state
{
	Texture = <g_Diffuse2Texture>;
	MinFilter = Linear;
	MagFilter = Linear;
	//MipFilter = Point;
  AddressU = <g_Diffuse2_AddressU>;
  AddressV = <g_Diffuse2_AddressV>;
};


sampler2D NormalSampler = sampler_state
{
	Texture = <g_NormalTexture>;

	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = Anisotropic;
	MaxAnisotropy = 16;
  AddressU = <g_Diffuse2_AddressU>;
  AddressV = <g_Diffuse2_AddressV>;
};

//Wind

float3 g_WindDirection
<
	string UIName = "Wind Direction";
	float UIMin = -1.0f;
	float UIMax = 1.0f;
	float UIStep = 0.01f;    
>
= float3(1.0f,1.0f,1.0f);
float g_WindForce
<
	string UIName = "Wind Force";
	float UIMin = 0.0f;
	float UIMax = 100.0f;
	float UIStep = 0.01f;    
>
= 0.01f;
float3 getWindVector()
{
	g_WindDirection.z*=0.3f;
#ifdef _3DSMAX_
	return normalize(g_WindDirection)*UnitScale(g_WindForce)*sin(1.5f*time);
#else
	return normalize(g_WindDirection)*g_WindForce*sin(1.5f*time);
	//float3 v=g_WindDirection*time;
	//v=sin(v);
	//return v*g_WindForce;
#endif
}
#define g_WindVector getWindVector()


bool g_WindEnable <
	string UIName = "Wind Enable";
> = false;
bool g_BillboardWindEnable <
	string UIName = "Billboard Wind Enable";
> = false;
float g_Solidity
<
	string UIName = "Solidity";
	float UIMin = 0.0f;
	float UIMax = 100.0f;
	float UIStep = 0.01f;
>
= 0.5f;

float g_BillboardSolidity
<
	string UIName = "Billboard Solidity";
	float UIMin = 0.0f;
	float UIMax = 100.0f;
	float UIStep = 0.01f;
>
= 1.0f;

float getBillboardSolidtity()
{
	return g_BillboardSolidity*1000.0f;
}

//Wind End

//cube ambient start
texture g_AmbientCubeTexture < 
	string UIName = "ambient cube";
	string ResourceType = "CUBE";
	//int Texcoord = 0;
	//int MapChannel = 1;
>;

float g_AmbientPower<
	string UIName = "ambient power";
	float UIMin = 0.0f;
	float UIMax = 2.0f;
	float UIStep = 0.01f;
>   = .2f;

samplerCUBE g_AmbientCubeSampler : register(s1) = sampler_state 
{
	Texture = <g_AmbientCubeTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};
//cube ambient end

int texcoord_UV1 : Texcoord
<
	int Texcoord = 0;
	int MapChannel = 1;
	string UIWidget = "None";
>;
int texcoord_UV2 : Texcoord
<
	int Texcoord = 1;
	int MapChannel = 2;
	string UIWidget = "None";
>;
int texcoord_UV3 : Texcoord
<
	int Texcoord = 2;
	int MapChannel = 3;
	string UIWidget = "None";
>;

int texcoord1 : Texcoord
<
	int Texcoord = 3;
	int MapChannel = 0;
	string UIWidget = "None";
>;

int texcoord2 : Texcoord
<
	int Texcoord = 4;
	int MapChannel = -2;
	string UIWidget = "None";
>;

float4 EditorParams = {0,1,0,0};

float getAlphaRef()
{
	return g_AlphaRef/255.0;
}
#define g_AlphaTestValue getAlphaRef()

float normalBumpPower = 1.0;

float4 g_AmbientColor : Ambient
<
	string UIName = "Ambient Color";
> = float4( 0.4f, 0.4f, 0.4f, 0.4f ); 

float4 g_EmissiveColor
<
	string UIName = "Emissive Color";
> = float4( 0.501961,0.501961,0.501961,1 ); 

float4 g_SpecularColor
<
	string UIName = "Specular Color";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f ); 

float g_Glossiness
<
	string UIName = "Glossiness";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 3.0f;	
> = 1.0f;

float g_Smoothness <
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 1.0;
    float UIStep = 0.001;
    string UIName =  "Smoothness";
> = -0.1;

bool g_NoLighting <
    string UIName = "No Light";
> = false;

bool g_ReflectionEnable <
	string UIName = "Reflection Enable";
> = false;

texture g_ReflectionTexture < 
	string UIName = "Reflection";
	string ResourceType = "CUBE";
	//int Texcoord = 0;
	//int MapChannel = 1;
>;

float g_ReflectionAmount <
	string UIName = "Reflection Amount";
	float UIMin = 0.0f;
	float UIMax = 2.0f;
	float UIStep = 0.01f;
>   = .2f;

samplerCUBE ReflectionSampler : register(s0) = sampler_state 
{
	Texture = <g_ReflectionTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

float4 HDRParam = {1,0,0,0};
float4 g_LightingParams = {1,1,1,1};
float4 g_ScatterColor = float4( 0, 0, 0, 0 );
float4 use_instance_rendering = {0,0,0,0};
float g_StageStyle = 1.0;
float4 g_ColourFading = float4(1.0, 1.0, 1.0, 1.0);