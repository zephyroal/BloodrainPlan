#ifndef _JoyLightMaterial_INC_
#define _JoyLightMaterial_INC_

#include "JoyLightMateria_DebugSwitchl.inc"
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
#endif
struct VS_IN
{
	float4 	position 	: POSITION;
	float2 	uv1				: TEXCOORD0;
};

struct VS_OUT
{
	float4 	hPos			: POSITION;
	float4  uv1				: TEXCOORD0;
	float4  v				: TEXCOORD1;
	float4  pcw				: TEXCOORD2;
	float4  pccw			: TEXCOORD3;
	float4	ray				: TEXCOORD4;
};

// Use For MRT
struct PS_OUT
{
	float4	color1		: COLOR0;
	float4  color2		: COLOR1;
};


#endif