#ifndef _JoyParticleMaterial_INC_
#define _JoyParticleMaterial_INC_

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
#include "JoyDistortion.inc"
#include "JoyComplexity.inc"
#endif
struct VS_IN
{
	float4 	position 	: POSITION;
	float2 	uv1				: TEXCOORD0;
	float4  color			: COLOR0;
};

struct VS_OUT
{
	float4 	hPos			: POSITION;
	float4  uv1				: TEXCOORD0;
#if g_Diffuse2Enable == 1
  float4  uv2				: TEXCOORD1;
#endif

#if g_SpaceDistortionEnable == 1
	float4  uv3       : TEXCOORD2;
	float4  pv        : TEXCOORD3;
#endif

	float4  col				: COLOR;
};

// Use For MRT
struct PS_OUT
{
	float4	color1		: COLOR0;
	float4	color2		: COLOR1;
};


#endif