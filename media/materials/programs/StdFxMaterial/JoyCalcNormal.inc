#ifndef _JoyCalcNormal_INC_
#define _JoyCalcNormal_INC_

#ifdef _3DSMAX_
#define g_NormalMapEnable 1
#endif

void Calc_Normal( inout VertParams params )
{
#if g_NormalMapEnable == 1 || g_NormalLayerEnable == 1
	params.vTangent 	= MUL( world_matrix, params.VI.tangent );
	params.vBinormal 	= float4( cross( params.vTangent, params.vNormal ), 0 );
#endif
}

#endif