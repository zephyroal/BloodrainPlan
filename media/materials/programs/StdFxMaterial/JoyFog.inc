#ifndef _JoyFog_INC_
#define _JoyFog_INC_

void Calc_Fog_Factor( inout VertParams params )
{
		float factor = saturate( ( fog_params.z + params.vView.z ) * fog_params.w );
		params.fog = float4( fog_colour.rgb, factor );
}

void Calc_Fog_Color( inout FragParams params )
{
#if g_FogEnable == 1
		params.cFinal.rgb = lerp( params.FI.fog.rgb, params.cFinal.rgb, params.FI.fog.a );
#endif
}

#endif