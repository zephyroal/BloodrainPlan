#ifndef _JoyDiffuse2Blend_INC_
#define _JoyDiffuse2Blend_INC_

void Calc_Diffuse2_Blend( inout FragParams params )
{
#ifdef _3DSMAX_

	if( g_Diffuse2Enable )
	{
		float4 c = tex2D( Diffuse2Sampler, params.FI.baseTC );
		c = Gamma( c, params );
		if( g_Diffuse2Enable )
		{
			if( g_Diffuse2Add )
			{
				params.cDiffuseRT += c;
			}
			else if( g_Diffuse2Mul )
			{
				params.cDiffuseRT *= c;
			}
		  else 
		  {
				// todo : blend layer
			}
		}
	}

#else

#if g_Diffuse2Enable == 1
	float4 c = tex2D( Diffuse2Sampler, params.FI.extTC1.xy );
	c = Gamma( c, params );
	#if g_Diffuse2BlendType == 0
		params.cDiffuseRT = c * params.cDiffuseRT;
	#endif
	#if g_Diffuse2BlendType == 1
		params.cDiffuseRT += c;
	#endif
	#if g_Diffuse2BlendType == 2
		params.cDiffuseRT -= c;
	#endif
	#if g_Diffuse2BlendType == 3
		float factor = tex2D( BlendSampler, params.FI.baseTC ).r;
		params.cDiffuseRT = lerp( params.cDiffuseRT, c, factor );
	#endif
#endif

#endif

	params.cFinal = params.cDiffuseRT;
}

#endif