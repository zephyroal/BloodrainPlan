#ifndef _JoyTexDensity_INC_
#define _JoyTexDensity_INC_

#include "JoyCommon.inc"
#include "JoySkin.inc"
#include "JoyCalcPos.inc"
#include "JoyCalcUV.inc"
#ifdef __PS__
#include "JoyDiffuse.inc"
#include "JoyTexKill.inc"
#endif
struct VS_IN
{
	float4 	position 	: POSITION;
	float2 	uv1				: TEXCOORD0;
#if g_UseSkin == 1
	float4	blendIdx	: BLENDINDICES;
	float4	Weight		: BLENDWEIGHT;
#endif
#if g_BeastMapEnable == 1
	float2  uv2				: TEXCOORD1;
#endif
};

struct VS_OUT
{
	float4 	hPos			: POSITION;
	float4  uv1				: TEXCOORD0;
	float4  pw				: TEXCOORD1;
};

struct PS_OUT
{
	float4	color1		: COLOR0;
};

#endif