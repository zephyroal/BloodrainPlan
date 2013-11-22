#ifndef _JoyBasicMaterialVS_H_
#define _JoyBasicMaterialVS_H_

// Define In VS
#define __VS__

#include "JoyBasicMaterial.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.normal			= float4( IN.normal.rgb, 0 );
	VI.tc1				= IN.uv1.xyxy;
#if g_UseSkin == 1
	VI.blendIdx		= IN.blendIdx;
	VI.weight			= IN.Weight;
#endif
#if g_GrassEnable != 0
  VI.normal.w   = IN.position.w;
#endif
#if g_NormalMapEnable == 1
	VI.tangent		= float4( IN.tangent, 0 );
#endif
#if g_BeastMapEnable == 1
	VI.tc2				= IN.uv2.xyxy;
#endif
#if g_UseVertexColor == 1
	VI.vertexColor= IN.color;
#endif


	return VI;
}

VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= params.ps;
	OUT.uv1				= params.baseTC;
	OUT.normal		= params.vNormal;
	OUT.v					= float4( params.vView.xyz, params.ps.z / params.ps.w );
	OUT.pw				= float4( params.pw.xyz, -params.pv.z );
	
#if g_UseVertexColor == 1
	OUT.col				= params.VI.vertexColor;
#else
	OUT.col				= float4(1.0f,1.0f,1.0f,1.0f);
#endif
#if g_NormalMapEnable == 1
	OUT.tangent		= params.vTangent;
	OUT.binormal	= params.vBinormal;
#endif

#if g_Diffuse2Enable == 1
	OUT.uv2				= params.externTC;
#endif
	
	return OUT;
}

VS_OUT main( VS_IN IN )
{
	VertParams params	= (VertParams)0;
	params.VI = fmt_input( IN );
	
	/// calc position
	Calc_Position( params );
	
	/// calc normal
	Calc_Normal( params );
	
	/// calc uv
	Calc_Uv( params );
	
	/// calc view
	Calc_View( params );
	
	return fmt_output( params );
}

#endif