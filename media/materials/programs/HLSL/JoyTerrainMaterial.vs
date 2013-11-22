#ifndef _JoyTerrainMaterialVS_H_
#define _JoyTerrainMaterialVS_H_

// Define In VS
#define __VS__

#include "JoyTerrainMaterial.inc"

VertInput fmt_input( VS_IN IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.position.xyz, 1 );
	VI.normal			= float4( IN.normal.rgb, 0 );
#if g_NormalLayerEnable == 1
	VI.tangent		= float4( IN.tangent.rgb, 0 );
#endif
	VI.tc1				= IN.uvBottom;
#if g_MiddleLayerEnable == 1
	VI.tc2				= IN.uvMiddle;
#endif
#if g_UpperLayerEnable == 1
	VI.tc3				= IN.uvUpper;
#endif
#if g_DetailLayerEnable == 1
	VI.tc4				= IN.uvDetail;
#endif
#if g_NormalLayerEnable == 1
	VI.tc5				= IN.uvNormal;
#endif
#if g_BlendLayerEnable == 1 || g_ColorMapEnable == 1
	VI.tc7				= IN.uvBlendLightColor;
#endif
#if g_LightMapEnable == 1
	VI.tc1.zw			= IN.uvBlendLightColor.xy;
#endif	
	return VI;
}

VS_OUT fmt_output( VertParams params )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.hPos 			= mul( view_matrix, params.pw );
	OUT.hPos      = mul( projection_matrix, OUT.hPos );
	OUT.normal		= params.vNormal;
#if g_NormalLayerEnable == 1
	OUT.tangent		= params.vTangent;
	OUT.biNormal	= params.vBinormal;
#endif
	OUT.v					= float4( params.vView.xyz, params.ps.z / params.ps.w );
	OUT.pw				= float4( params.pw.xyz,-params.pv.z );
	// OUT.fog				= params.fog;
	
	OUT.uvBottom	= params.VI.tc1;
#if g_MiddleLayerEnable == 1
	OUT.uvMiddleUpper.xy		 = params.VI.tc2.xy;
#endif

#if g_UpperLayerEnable == 1
	OUT.uvMiddleUpper.zw		 = params.VI.tc3.xy;
#endif

#if g_DetailLayerEnable == 1
	OUT.uvDetailNormal.xy 	 = params.VI.tc4.xy;
#endif

#if g_NormalLayerEnable == 1
	OUT.uvDetailNormal.zw    = params.VI.tc5.xy;
#endif

#if g_BlendLayerEnable == 1 || g_ColorMapEnable == 1
	OUT.uvBlendLightColor.xy = params.VI.tc7.xy;
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
	
	/// calc view
	Calc_View( params );
	
	/// calc fog
	// Calc_Fog_Factor( params );
	
	return fmt_output( params );
}

#endif