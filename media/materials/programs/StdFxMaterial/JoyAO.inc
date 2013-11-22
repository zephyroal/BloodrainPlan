#ifndef _JoyAO_INC_
#define _JoyAO_INC_

#include "JoyCommon.inc"
#include "JoySkin.inc"
#include "JoyCalcPos.inc"
#include "JoyCalcUV.inc"

struct VS_IN
{
		float4 position		: POSITION;
};
struct VS_OUT
{
    float4 hPos				: POSITION;
    float2 uv1 				: TEXCOORD0;		
    float4 ray				: TEXCOORD1;				 
};

struct PS_OUT
{
		float4 color1   	: COLOR0;
};

#endif