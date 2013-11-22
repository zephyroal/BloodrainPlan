bool g_GlobalFogEnable <
    string UIName = "Fog Enable(Global)";
> = false;

bool g_FogEnable <
    string UIName = "Fog Enable";
> = true;

float4 g_FogColor
<
	string UIName = "Fog Color";
> = float4( 0.2f, 0.3f, 0.5f, 1.0f ); 

//雾的开始衰减距离（米）
float g_FogParam_1
<
	string UIName = "Fog Param 1";
	string UIType = "FloatSpinner";
	float UIMin =  0.0f;
	float UIStep = 1.0;
	float UIMax =  1000.0f;
> = 60.0f;

//雾的衰减距离（米)
float g_FogParam_2
<
	string UIName = "Fog Param 2";
	string UIType = "FloatSpinner";
	float UIMin =  0.0f;
	float UIStep = 1.0;
	float UIMax =  1000.0f;
> = 120.0f;

//雾的衰减速度：指数
float g_FogParam_3
<
	string UIName = "Fog Param 3";
	string UIType = "FloatSpinner";
	float UIMin =  0.0f;
	float UIStep = 1.0;
	float UIMax =  1000.0f;
> = 1.0f;

float4 get_FogParam()
{
#ifdef _3DSMAX_
	return float4(
		UnitScale(g_FogParam_1),
		1.0/UnitScale(g_FogParam_2),
		g_FogParam_3,
		0);
#else
	return float4(
		g_FogParam_1,
		1.0/g_FogParam_2,
		g_FogParam_3,
		0);
#endif
}

#define g_FogParam get_FogParam()



void calc_fog_in_view_space(float3 P,out float rate,out float4 fog_color)
{
	rate=0;
	fog_color=g_FogColor;
}

void calc_fog_in_world_space(float3 P,out float rate,out float4 fog_color)
{
	rate=(length(camera_position.xyz-P)-g_FogParam.x)*g_FogParam.y;
	rate=saturate(rate);
	rate=pow(rate,g_FogParam.z);
	fog_color=g_FogColor;
}

void calc_and_blend_fog_in_view_space(float3 P,float4 scene_color,out float4 result_color)
{
	float rate;
	float4 fog_color;
	calc_fog_in_view_space(P,rate,fog_color);
	result_color=lerp(scene_color,fog_color,rate);
}





//depth fog information
bool g_DepthFogEnable <
    string UIName = "Fog Enable(Depth)";
> = false;
texture g_DepthFogTexture
<
	string ResourceName = "";
    string UIName =  "Material Texture";
    string ResourceType = "2D";
>;
sampler2D g_DetphFogSampler = sampler_state
{
	Texture = <g_DepthFogTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
};

