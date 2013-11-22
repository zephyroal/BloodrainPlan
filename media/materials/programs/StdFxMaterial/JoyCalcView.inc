#ifndef _JoyCalcView_INC_
#define _JoyCalcView_INC_

/// calc view dir
void Calc_View( inout VertParams params )
{
#ifdef _3DSMAX_
		params.vView = float4( camera_position - params.pw.xyz, 1 );
#else
		params.vView = camera_position - params.pw * camera_position.w;
#endif
}

#endif