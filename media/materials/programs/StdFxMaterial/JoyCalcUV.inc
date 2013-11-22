#ifndef _JoyCalcUV_INC_
#define _JoyCalcUV_INC_

void Calc_Uv( inout VertParams params )
{
#ifdef _3DSMAX_
		params.VI.tc1.xy = float2( params.VI.tc1.x, 1.0 + params.VI.tc1.y );
		params.baseTC.xy = params.VI.tc1.xy;
		
		float my_time = g_UV1Time;
		if ( g_UV1RotateEnable )
		{
				float uv_rotate =- g_UV1Rotate * my_time;
		
				float4x4 rot = identity_matrix;
				float theta = uv_rotate.x * 2.0f * 3.14159f;
			
				float cosTheta = cos(theta);
				float sinTheta = sin(theta);
		
				rot[0][0] = cosTheta;
				rot[0][1] = -sinTheta;
				rot[1][0] = sinTheta;
				rot[1][1] = cosTheta;
			
				// Offset center of rotation to center of texture
				rot[0][3] = 0.5 + ( (-0.5 * cosTheta) - (-0.5 * sinTheta) );
				rot[1][3] = 0.5 + ( (-0.5 * sinTheta) + (-0.5 * cosTheta) );
			
				float4 uv = float4(params.baseTC.xy,0,1);
				float4 r = mul(rot,uv);
				params.baseTC.xy = r.xy;
		}
		else if(g_UV1ScrollEnable)
		{
				float2 uv_scroll = float2(g_U1Scroll,-g_V1Scroll) * my_time;
				params.baseTC.xy = params.baseTC.xy + uv_scroll;
		}
#else

#if g_Texture1EffectEnable == 1
		params.baseTC.xy = mul( texture_matrix_0, float4( params.VI.tc1.xy, 1, 1 ) ).xy;
#else
		params.baseTC.xy = params.VI.tc1.xy;
#endif

		params.baseTC.zw = params.VI.tc1.xy;
		
#if g_TexDistortionEnable == 1 || g_SpaceDistortionEnable == 1
		params.baseTC.zw = mul( texture_matrix_1, float4( params.baseTC.zw, 1, 1 ) ).xy;
#endif
		
#if g_Texture2EffectEnable == 1
		params.externTC.xy = mul( texture_matrix_2, float4( params.VI.tc1.xy, 1, 1 ) ).xy;
#else
		params.externTC.xy = params.VI.tc1.xy;
#endif
		
#if g_BeastMapEnable == 1
		params.VI.tc2.xy = params.VI.tc2.xy * BeastMapBias.zz + BeastMapBias.xy;
		params.baseTC.zw = params.VI.tc2.xy;
#endif

#endif
}

void Calc_ScreenUV( inout VertParams params )
{
		float4x4 scalemat = float4x4( 0.5,   0,   	0, 		0.5, 
	                                0,		-0.5,   0, 		0.5,
								   							  0,   	0, 			0.5, 	0.5,
								   							  0,   	0,   		0,   	1 );
		params.projTC = mul( scalemat, params.ps );
}

#endif