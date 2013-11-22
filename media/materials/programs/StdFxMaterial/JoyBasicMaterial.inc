#ifndef _JoyBasicMaterial_INC_
#define _JoyBasicMaterial_INC_

#include "JoyBasicMateria_DebugSwitchl.inc"
#include "JoyCommon.inc"
#include "JoyLight.inc"
#include "JoyFog.inc"
#include "JoySkin.inc"
#include "JoyGrass.inc"
#include "JoyCalcPos.inc"
#include "JoyCalcNormal.inc"
#include "JoyCalcUV.inc"
#include "JoyCalcView.inc"
#ifdef __PS__
#include "JoyDiffuse2Blend.inc"
#include "JoyNormalMap.inc"
#include "JoyFog.inc"
#include "JoyDiffuse.inc"
#include "JoyTexKill.inc"
#include "JoyBeast.inc"
#include "JoyComplexity.inc"
#endif

struct VS_IN
{
	float4 	position 	: POSITION;
	float2 	uv1				: TEXCOORD0;
	float4  normal		: NORMAL;
#if g_UseSkin == 1
	float4	blendIdx	: BLENDINDICES;
	float4	Weight		: BLENDWEIGHT;
#endif
#if g_NormalMapEnable == 1
	float3	tangent		: TANGENT0;
#endif
#if g_BeastMapEnable == 1
	float2  uv2				: TEXCOORD1;
#endif
#if g_UseVertexColor
	float4  color			: COLOR0;
#endif

};

struct VS_OUT
{
	float4 	hPos			: POSITION;
	float4  uv1				: TEXCOORD0;
	float4 	normal		: TEXCOORD2;
	float4  v					: TEXCOORD3;
	float4  pw				: TEXCOORD4;
#if g_NormalMapEnable == 1
	float3	tangent		: TEXCOORD5;
	float3 	binormal  : TEXCOORD6;
#endif
	float4  col				: COLOR;
#if g_Diffuse2Enable == 1
  float4  uv2				: TEXCOORD7;
#endif
};

// Use For MRT
struct PS_OUT
{
	float4	color1		: COLOR0;
	float4 	color2		: COLOR1;
};


#endif