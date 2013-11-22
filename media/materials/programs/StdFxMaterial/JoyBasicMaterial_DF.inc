#ifndef _JoyBasicMaterial_INC_
#define _JoyBasicMaterial_INC_

#include "JoyBasicMateria_DebugSwitchl_DF.inc"
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
#include "JoyBeast.inc"
#include "JoyComplexity.inc"
#endif

struct VS_IN
{
	float4 	position 	: POSITION;
	float4	blendIdx	: BLENDINDICES;
	float4	Weight		: BLENDWEIGHT;
	float4  normal		: NORMAL;
	float3	tangent		: TANGENT0;
	float2 	uv1				: TEXCOORD0;
#if g_BeastMapEnable == 1
	float2  uv2				: TEXCOORD1;
#endif
#if g_UseVertexColor
	float4  color			: COLOR0;
#endif

	float4  world1		: TEXCOORD3;
	float4  world2		: TEXCOORD4;
	float4  world3		: TEXCOORD5;
	float4	world4		: TEXCOORD6;
};

struct VS_OUT
{
	float4 	hPos			: POSITION;
	float4 	fog				: FOG;
	float4  uv1				: TEXCOORD0;
	float4 	normal		: TEXCOORD1;
	float4  pv				: TEXCOORD2;
	float3	tangent		: TEXCOORD3;
	float3 	binormal  : TEXCOORD4;
	float4  col				: COLOR;
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