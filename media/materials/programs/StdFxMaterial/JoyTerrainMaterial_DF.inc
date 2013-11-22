#ifndef _JoyTerrainMaterial_INC_
#define _JoyTerrainMaterial_INC_

#include "JoyTerrainMaterial_DebugSwitch_DF.inc"
#include "JoyCommon.inc"
#include "JoyLight.inc"
#include "JoyFog.inc"
#include "JoySkin.inc"
#include "JoyGrass.inc"
#include "JoyCalcPos.inc"
#include "JoyCalcNormal.inc"
#include "JoyCalcUV.inc"
#include "JoyCalcView.inc"
#include "JoyCodec.inc"
#ifdef __PS__
#include "JoyDiffuse2Blend.inc"
#include "JoyNormalMap.inc"
#include "JoyFog.inc"
#include "JoyDiffuse.inc"
#include "JoyTexKill.inc"
#include "JoyColorMap.inc"
#include "JoyBeast.inc"
#endif
struct VS_IN
{
	float4 	position 	: POSITION;
	float4  normal		: NORMAL;
	// 单层uv
	float4 	uvBottom				: TEXCOORD0;
	
#if g_MiddleLayerEnable == 1
		float4  uvMiddle			: TEXCOORD1;
#endif

#if g_UpperLayerEnable == 1
		float4  uvUpper				: TEXCOORD2;
#endif

#if g_DetailLayerEnable == 1
		float4  uvDetail			: TEXCOORD3;
#endif

#if	g_NormalLayerEnable == 1
		float4  uvNormal			: TEXCOORD4;
#endif

#if	g_BlendLayerEnable == 1 || g_ColorMapEnable == 1 
		float4  uvBlendLightColor	: TEXCOORD7;
#endif
};

struct VS_OUT
{
	float4 	hPos								: POSITION;
	float4 	fog									: FOG;
	float4  uvBottom						: TEXCOORD0;		
	
#if g_MiddleLayerEnable == 1 || g_UpperLayerEnable == 1
	float4	uvMiddleUpper 			: TEXCOORD1; 		//Middle用xy，Upper用zw
#endif

#if g_BlendLayerEnable == 1 || g_ColorMapEnable == 1
	float4  uvBlendLightColor		: TEXCOORD2;		//Blend用xy，Light和Color共用zw
#endif

#if g_DetailLayerEnable == 1 || g_NormalLayerEnable == 1
	float4	uvDetailNormal			: TEXCOORD3;		//Detail用xy，Normal用zw
#endif

	float4 	normal							: TEXCOORD4;
	float4  l										: TEXCOORD5;
	float4  v										: TEXCOORD6;
	float4  pv									: TEXCOORD7;
};

// Use For MRT
struct PS_OUT
{
	float4	color1		: COLOR0;
	float4 	color2		: COLOR1;
	float4  color3		: COLOR2;
	float4  color4		: COLOR3;
};

#endif