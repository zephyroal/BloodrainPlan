#ifndef _JoyParallax_INC_
#define _JoyParallax_INC_

void Clac_Parallax( inout FragParams params )
{
#ifdef _3DSMAX_

	if ( g_ParallaxEnable && g_NormalEnable )
	{
			float3 V = normalize( params.FI.vView );
			float height = tex2D( NormalSampler, params.FI.baseTC.xy ).w;
			float Altitude = height + g_ParallaxBias; 
			float2 offset_uv = Altitude * g_ParallaxScale * V;
    
			params.FI.baseTC.xy += offset_uv;
	}

#else
#endif
}

#endif