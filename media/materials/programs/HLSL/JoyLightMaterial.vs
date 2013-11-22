#ifndef _JoyLightMaterialVS_H_
#define _JoyLightMaterialVS_H_

// Define In VS
#define __VS__

#include "JoyLightMaterial.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.tc1				= IN.uv1.xyxy;

	return VI;
}

VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	
	OUT.hPos 			= params.ps;
	OUT.uv1				= params.projTC;
	OUT.v				= float4( params.vView.xyz, params.ps.z / params.ps.w );
	OUT.pcw				= params.pv;
	OUT.pccw			= params.externTC;
	OUT.ray				= mul(inverse_projection_matrix, params.ps);
	return OUT;
}

VS_OUT main( VS_IN IN )
{
	VertParams params	= (VertParams)0;
	params.VI = fmt_input( IN );

#if g_LightType == 1 || g_LightType == 9
	/// calc position
	Calc_Quad_Position( params );
#else
	/// calc position
	Calc_Position( params );
#endif
	
	/// calc view
	Calc_View( params );
	
	/// Calc Screen UV
	Calc_ScreenUV( params );
	
	return fmt_output( params );
}

#endif