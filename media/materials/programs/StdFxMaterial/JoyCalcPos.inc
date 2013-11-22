#ifndef _JoyCalcPos_INC_
#define _JoyCalcPos_INC_

/// convert object-space to world space
/// input 	: po
/// output	: pw, pv, ps
void Calc_Position( inout VertParams params )
{
	/// calc grass
#if g_GrassEnable != 0
	params.VI.position = calc_grass_pos( grassOffset, params.VI.position, params.VI.tc1.xy, grassParam );
#else
	params.VI.position = float4( params.VI.position.xyz, 1 );
#endif

	params.po = params.VI.position;
	
	/// calc hw skin
	Calc_Skin( params );
	
	/// position in view & projection
	params.pv = MUL( view_matrix, params.pw );
	params.ps = MUL( projection_matrix, params.pv );
}

void Calc_Quad_Position( inout VertParams params )
{
	params.VI.position.xy = sign( params.VI.position.xy );
	params.po = float4( params.VI.position.xy, 0, 1 );
	params.pw = params.po;
	params.pv = params.po;
	params.ps = params.po;
}
#endif