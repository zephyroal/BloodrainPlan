#ifndef _JoyTexDensityVS_H_
#define _JoyTexDensityVS_H_

// Define In VS
#define __VS__

#include "JoyTexDensity.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.tc1				= IN.uv1.xyxy;
#if g_UseSkin == 1
	VI.blendIdx		= IN.blendIdx;
	VI.weight			= IN.Weight;
#endif
#if g_BeastMapEnable == 1
	VI.tc2				= IN.uv2.xyxy;
#endif
	return VI;
}


VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= params.ps;
	OUT.uv1				= params.baseTC;
	OUT.pw				= float4( params.pw.xyz, -params.pv.z );
	return OUT;
}

VS_OUT main( VS_IN IN )
{
	VertParams params	= (VertParams)0;
	params.VI = fmt_input( IN );
	
	/// calc position
	Calc_Position( params );
	
	/// calc uv
	Calc_Uv(params);
	
	return fmt_output( params );
}

#endif