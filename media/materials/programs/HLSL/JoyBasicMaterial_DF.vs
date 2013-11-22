#ifndef _JoyBasicMaterialVS_H_
#define _JoyBasicMaterialVS_H_

// Define In VS
#define __VS__

#include "JoyBasicMaterial_DF.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.normal		= float4( IN.normal.rgb, 0 );
	
#if g_GrassEnable != 0
	VI.normal.w   = IN.position.w;
#endif

#if g_UseSkin == 1	
	VI.blendIdx		= IN.blendIdx;
	VI.weight		= IN.Weight;
#endif

	VI.tangent		= float4( IN.tangent, 0 );
	VI.tc1			= IN.uv1.xyxy;
	
#if g_BeastMapEnable == 1
	VI.tc2			= IN.uv2.xyxy;
#endif

#if g_UseVertexColor
	VI.vertexColor= IN.color;
#endif

	VI.tc3 = IN.world1;
	VI.tc4 = IN.world2;
	VI.tc5 = IN.world3;
	VI.tc6 = IN.world4;

	return VI;
}

VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= params.ps;
	OUT.uv1				= params.baseTC;
#if g_UseVertexColor == 1
	OUT.col				= params.VI.vertexColor;
#else
	OUT.col				= float4(1.0f,1.0f,1.0f,1.0f);
#endif
	OUT.pv				= float4( -params.pv.xyz, 1 );
	OUT.normal		= MUL( view_matrix, float4(params.vNormal.xyz,0) );

#if g_GrassEnable != 0
	OUT.normal.w   = params.vNormal.w;
#endif
	
	OUT.tangent		= MUL( view_matrix, float4(params.vTangent.xyz,0) );
	OUT.binormal	= MUL( view_matrix, float4(params.vBinormal.xyz,0) );
	
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
	
	return fmt_output( params );
}

#endif