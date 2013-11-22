#ifndef _JoyAOVS_H_
#define _JoyAOVS_H_

// Define In VS
#define __VS__

#include "JoyAO.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	return VI;
}

VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= params.ps;
	OUT.uv1				= params.projTC;
	OUT.ray				= params.ps;
	return OUT;
}

VS_OUT main( VS_IN IN )
{
	VertParams params	= (VertParams)0;
	params.VI = fmt_input( IN );
	
	/// calc position
	Calc_Quad_Position( params );
	
	/// calc uv
	Calc_ScreenUV( params );
	
	return fmt_output( params );
}

#endif