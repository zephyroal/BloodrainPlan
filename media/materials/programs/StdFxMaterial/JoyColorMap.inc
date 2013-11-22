#ifndef _JoyColorMap_INC_
#define _JoyColorMap_INC_

void Calc_Color_Blend( inout FragParams params )
{
#if g_ColorMapEnable == 1
		float4 colorMapColor = tex2D( ColorMapSampler, params.FI.extTC2.xy );
		colorMapColor = Gamma( colorMapColor, params );
		params.cFinal.rgb = OverlayBlend( params.cFinal.rgb, colorMapColor.xyz ).rgb;
#endif
}

#endif