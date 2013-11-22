#ifndef _JoyBeast_INC_
#define _JoyBeast_INC_

///
void Calc_BeastMap_Blend( inout FragParams params )
{
#if g_BeastMapEnable == 1 || g_LightMapEnable == 1
	float4 beastMap = tex2D( BeastmapSampler, params.FI.baseTC.zw );
  	beastMap = BeastLightRange.x + beastMap * ( BeastLightRange.y - BeastLightRange.x );
  	params.cFinal.rgb *= ( beastMap.rgb + surface_emissive_colour.rgb );
#endif
}

void Calc_BeastMap_Blend_Df( inout FragParams params )
{
	params.cAddionalRT = 0;
#if g_BeastMapEnable == 1 || g_LightMapEnable == 1
	float4 beastMap = tex2D( BeastmapSampler, params.FI.baseTC.zw );
  	beastMap = BeastLightRange.x + beastMap * ( BeastLightRange.y - BeastLightRange.x );
  	params.cAddionalRT.rgb = beastMap.rgb + surface_emissive_colour.rgb;
#endif
}

#endif