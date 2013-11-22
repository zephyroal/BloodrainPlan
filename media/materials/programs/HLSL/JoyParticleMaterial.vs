#ifndef _JoyParticleMaterialVS_H_
#define _JoyParticleMaterialVS_H_

// Define In VS
#define __VS__

#include "JoyParticleMaterial.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.tc1				= IN.uv1.xyxy;
	VI.vertexColor= IN.color;

	return VI;
}

VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= params.ps;
	OUT.uv1				= params.baseTC;
	
#if g_Diffuse2Enable == 1
	OUT.uv2				= params.externTC;
#endif

#if g_SpaceDistortionEnable == 1
	OUT.uv3       = params.projTC;
	OUT.pv        = float4(-params.pv.z, 1.0f, 1.0f, 1.0f);
#endif

	OUT.col				= params.VI.vertexColor;

	return OUT;
}

VS_OUT main( VS_IN IN )
{
	VertParams params	= (VertParams)0;
	params.VI = fmt_input( IN );
	
	/// calc position
	Calc_Position( params );
	
#if g_SpaceDistortionEnable == 1
  Calc_ScreenUV( params );
#endif
	
	/// calc uv
	Calc_Uv( params );
	
	return fmt_output( params );
}

#endif