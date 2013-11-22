#ifndef _JoySceneDepth_INC_
#define _JoySceneDepth_INC_

#include "JoyCommon.inc"
#include "JoySkin.inc"
#include "JoyGrass.inc"
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
};

struct VS_OUT
{
	float4 	hPos 			: POSITION;
	float4  depth			: TEXCOORD1;
};

struct PS_OUT
{
	float4	color1		: COLOR0;
};

#endif