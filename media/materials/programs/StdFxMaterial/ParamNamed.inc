#ifndef _JoyParamNamed_INC_
#define _JoyParamNamed_INC_

/// 防止部分dx版本中不使用的变量仍然进行编译
#ifndef SD
#define SD s9
#endif

#ifndef SD2
#define SD2 s9
#endif

#ifndef SDM
#define SDM s9
#endif

#ifndef SNM
#define SNM s9
#endif

#ifndef SSP
#define SSP s9
#endif

#ifndef SL1
#define SL1 s9
#endif

#ifndef SBM
#define SBM s9
#endif

#ifndef SCC
#define SCC s9
#endif

#ifndef SL1
#define SL1 s9
#endif

#ifndef SL2
#define SL2 s9
#endif

#ifndef SL3
#define SL3 s9
#endif

#ifndef SLDetail
#define SLDetail s9
#endif

#ifndef SLNormal
#define SLNormal s9
#endif

#ifndef SLBlend
#define SLBlend s9
#endif

#ifndef SC
#define SC s9
#endif

#if g_UseSkin == 1
	float4 g_BoneNum 		= {0,0,0,0};
#endif

#if g_GrassEnable != 0
	float4 grassOffset	= {0,0,0,0};
	float4 grassParam		= {0,0,0,0};
#endif

#if g_AlphaTestEnable == 1 
	float AlphaRef;
#endif

#if g_NormalMapEnable == 1
	sampler NormalSampler 	: register(SNM);
	float normalBumpPower;
#endif

float4 g_LightingParams;

// 全局控制开关
float4 EditorParams = {0,0,0,0};
// 场景人物光照控制参数
float4 ActorParams ={ 1,1,1,1 };
// 场景复杂度控制参数
float4 ComplexityParams ={ 0,0,0,0 };


// HDR系数
float4 HDRParam = { 1,0,0,0 };

#if g_BeastMapEnable == 1 || g_LightMapEnable == 1
float4 BeastMapBias = {0,0,0,0};
#endif

#ifdef __PS__
/// 地形
sampler BottomLayerSampler 		: register(SL1);
#if g_MiddleLayerEnable == 1
	sampler MiddleLayerSampler 	: register(SL2);
#endif

#if g_UpperLayerEnable == 1
	sampler UpperLayerSampler 	: register(SL3);
#endif

#if g_DetailLayerEnable == 1
	sampler DetailLayerSampler 	: register(SLDetail);
#endif

#if g_NormalLayerEnable == 1
	sampler NormalLayerSampler 	: register(SLNormal);
#endif

#if g_BlendLayerEnable == 1
	sampler BlendLayerSampler 	: register(SLBlend);
#endif

#if g_ColorMapEnable == 1
	sampler ColorMapSampler 		: register(SC);
#endif

// 默认开启第一层diffuse贴图
#define g_Diffuse1Enable 1
#if g_Diffuse1Enable == 1
	sampler DiffuseSampler 	: register(SD);
#endif

#if g_Diffuse2Enable == 1
	sampler Diffuse2Sampler : register(SD2);
#endif

#if g_TexDistortionEnable == 1 || g_SpaceDistortionEnable == 1
  sampler DistortionSampler : register(SDT);
  float DistortionPower;
#endif

#if g_SpaceDistortionEnable == 1
  sampler SceneSampler : register(SDS);
#endif

#if g_Diffuse2BlendEnable == 1
	sampler BlendSampler 		: register(SDM);
#endif

#if g_SpecularMapEnable == 1
sampler SpecularSampler 	: register(SSP);
float specularMapPower;
#endif

#if g_BeastMapEnable == 1 || g_LightMapEnable == 1
sampler BeastmapSampler : register(SBM);
float4 BeastLightRange;
#endif

#if g_IsReleaseMode == 0
sampler ComplexitySampler : register(SCC);
#endif

#endif

float g_ReflectionAmount = 1.0;
float g_StageStyle = 1.0;
float4 g_ColourFading = float4(1.0, 1.0, 1.0, 1.0);


// scatter params
float4 	g_ScatterColor 		= float4( 0, 0.05, 0, 0 );
float4  g_ScatterParams = float4( 0.0, 0.6, 0.8, 0.0 ); // scatter, wrap, scatterWidth, minval

/// deffered shading params
float4	g_Diffuse 		= {0,0,0,0};
float4 	g_Specular 		= {0,0,0,0};
float4  g_Atten 		= {0,0,0,0};
float4  g_Direction 	= {0,0,0,0};
float4  g_Position	 	= {0,0,0,0};
float 	g_PowerScale 	= 1.f;

#if g_LightType == 2
	float4  g_Spot	 		= {0,0,0,0};
	#if g_SpotProjTexEnable == 1
		sampler SpotLightProjSampler : register(SPT);
		float4  g_SpotWvpMatrix0	 		= {1,0,0,0};
		float4  g_SpotWvpMatrix1	 		= {0,1,0,0};
		float4  g_SpotWvpMatrix2	 		= {0,0,1,0};
		float4  g_SpotWvpMatrix3	 		= {0,0,0,1};
	#endif
#endif

/// instance
float4 use_instance_rendering = {0,0,0,0};

#endif