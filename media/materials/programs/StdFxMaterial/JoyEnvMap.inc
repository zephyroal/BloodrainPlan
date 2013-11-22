#ifndef _JoyEnvMap_INC_
#define _JoyEnvMap_INC_

void Calc_EnvMap( inout FragParams params )
{
#ifdef _3DSMAX_

	if ( g_ReflectionEnable )
	{
			float3 CUV = reflect( normalize(params.FI.vView.xyz), normalize(params.FI.vNormal.rgb)).xzy;
			CUV.y = -CUV.y;
			float4 env = getEnvirionmentCMap( ReflectionSampler, CUV, g_ReflectionAmount );
			params.cFinal.rgb *= env.rgb;
	}

#else

	#if g_ReflectionMapEnable == 1
			float3 CUV = reflect(params.FI.vView.xyz, params.FI.vNormal.rgb).xzy;
			CUV.y = -CUV.y;
			float4 env = getEnvirionmentCMap( ReflectionSampler, CUV, g_ReflectionAmount );
			params.cFinal.rgb *= env.rgb;
	#endif
		
#endif
}

#endif