#ifndef _JoySceneDepthVS_H_
#define _JoySceneDepthVS_H_

// Define In VS
#define __VS__

#include "JoySceneDepth.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.tc1				= IN.uv1.xyxy;
#if g_UseSkin == 1
	VI.blendIdx		= IN.blendIdx;
	VI.weight			= IN.Weight;
#endif
	return VI;
}


VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= params.ps;
	OUT.depth 		= float4(params.ps.zw, params.baseTC.xy );

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