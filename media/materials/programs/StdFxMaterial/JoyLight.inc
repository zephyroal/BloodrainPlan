#ifndef _JoyLight_INC_
#define _JoyLight_INC_

#include "JoyLightModel.inc"

/// format light params
void Collect_Light_Params( inout FragParams params, int idx )
{
#ifdef _3DSMAX_
		params.LP.nType				= 0;
		params.LP.nIndex			= idx;
		params.LP.cDiffuse		= float3(1,1,1);
		params.LP.cSpecular		= float3(1,1,1);
		params.LP.vLight			= normalize( light_position_0.xyz - params.FI.position.xyz );
		params.LP.fNdotV			= dot( params.cNormalRT.rgb, params.FI.vView.xyz );
		params.LP.fNdotL			= dot( params.cNormalRT.rgb, params.LP.vLight );
		params.LP.fFallOff		= 1;
#else
		params.LP.nType				= 0;
		params.LP.nIndex			= idx;
		params.LP.cDiffuse		= light_diffuse_colour_power_scaled_array[idx].xyz;
		params.LP.cSpecular		= light_specular_colour_power_scaled_array[idx].xyz;
		params.LP.vLight  		= light_position_array[idx].xyz - params.FI.position.xyz * light_position_array[idx].w;
		params.LP.fNdotV			= dot( params.cNormalRT.rgb, params.FI.vView.xyz );
		params.LP.fNdotL			= dot( params.cNormalRT.rgb, normalize(params.LP.vLight) );
#if g_CapsLevel == 0 || g_BeastMapEnable == 1 || g_SM3 != 1
		params.LP.fFallOff		= 1.f;
#else
		params.LP.fFallOff		= pow( max( light_attenuation_array[idx].x - length(params.LP.vLight), 0 ) / ( light_attenuation_array[idx].x + 0.00001 ), light_attenuation_array[idx].y );
#endif
#endif
}

// 光照计算
void Calc_Light_Color_Ex( inout FragParams params, out float3 diffuse, out float3 specular )
{
#ifdef _3DSMAX_
		params.fSmoothness = g_Smoothness;
		params.fGlossiness = g_GlossnessTextureEnable ? params.cDiffuseRT.a * g_Glossiness : g_Glossiness;
		params.fShiness		 = 128;
#endif

/// 使用alpha通道作为高光系数  材质未实现
#if g_UseAlphaAsGlossiness == 1
		params.fGlossiness *= params.cDiffuseRT.a;
#endif

		float scatter = 0.0;
#if g_IsPlants == 1 && g_SM3 == 1 && g_CapsLevel != 0
		/*
		float NdotL_wrap = ( normalize(params.LP.fNdotL) + g_ScatterParams.y ) / ( 1 + g_ScatterParams.y );
		params.LP.fNdotL = clamp( NdotL_wrap, g_ScatterParams.w, 1 );
		scatter = smoothstep( 0, g_ScatterParams.z, NdotL_wrap ) * smoothstep( g_ScatterParams.z * 2, g_ScatterParams.z, NdotL_wrap );
		*/
		params.LP.fNdotL = smoothstep( 0, 1, max( (params.LP.fNdotL + g_ScatterParams.y) / ( 1 + g_ScatterParams.y ), g_ScatterParams.w ));
#endif
		
		specular = blinn( params.cNormalRT.rgb, params.FI.vView.xyz, normalize(params.LP.vLight), 128 );
#if g_SpecularMapEnable == 1
		specular *= tex2D( SpecularSampler, uv ).rgb * specularMapPower;
#endif

		diffuse 	= params.LP.fFallOff * max( params.LP.fNdotL, 0 ) * params.LP.cDiffuse.rgb * params.fSmoothness + scatter * g_ScatterColor.rgb;
		specular 	= params.LP.fFallOff * specular * params.LP.cSpecular.rgb * params.fGlossiness;
		// 暂时不使用高光 屏蔽计算
		specular = float3(0,0,0);
}

//
#ifdef __PS__

/// lighting in foward pipeline
/// input 	: [params.FI.position] [params.cNormalRT.rgb] [params.FI.vView] [params.LP.vLight]
/// output	: params.cFinal.rgb
void Calc_Light_In_Ps_FL( inout FragParams params )
{
#ifdef _3DSMAX_
	
	if ( g_NoLighting == 0 )
	{
			float3 lightDiffuseCol 	= float3(0,0,0);
			float3 lightSpecularCol = float3(0,0,0);
			
			Collect_Light_Params( params, 0 );
			Calc_Light_Color_Ex( params, lightDiffuseCol, lightSpecularCol );
			params.cFinal.rgb = params.cFinal.rgb * smoothstep( float3(0,0,0), float3(1.09,1.09,1.09), lightDiffuseCol + g_EmissiveColor ) + lightSpecularCol;
	}
	
#else

	#if g_NoLighting == 0 && (g_SM3 == 1)
	
			float3 lightDiffuseCol 	= float3(0,0,0);
			float3 lightSpecularCol = float3(0,0,0);
			float3 tmpDiffuse 			= float3(0,0,0);
			float3 tmpSpecular 			= float3(0,0,0);
			float3 diffuseSum       = float3(0,0,0);
			
			// avoid for unroll
	#if g_CapsLevel == 0 || g_BeastMapEnable == 1 || g_SM3 != 1
			Collect_Light_Params( params, 0 );
			Calc_Light_Color_Ex( params, tmpDiffuse, tmpSpecular );
			lightDiffuseCol += tmpDiffuse;
			lightSpecularCol += tmpSpecular;
			diffuseSum += params.LP.cDiffuse.rgb;
			Collect_Light_Params( params, 1 );
			Calc_Light_Color_Ex( params, tmpDiffuse, tmpSpecular );
			lightDiffuseCol += tmpDiffuse;
			lightSpecularCol += tmpSpecular;
			diffuseSum += params.LP.cDiffuse.rgb;
	#else
			for ( int i = 0; i < light_count; i++ )
				{
						Collect_Light_Params( params, i );
						Calc_Light_Color_Ex( params, tmpDiffuse, tmpSpecular );
						lightDiffuseCol += tmpDiffuse;
						lightSpecularCol += tmpSpecular;
						diffuseSum += params.LP.cDiffuse.rgb;
				}
	#endif

#if g_GrassEnable == 1
   lightDiffuseCol.rgb = max( lightDiffuseCol.rgb, params.LP.cDiffuse.rgb * 0.6 * params.fSmoothness);  
#elif g_GrassEnable == 2
	 lightDiffuseCol.rgb = max( lightDiffuseCol.rgb, diffuseSum * 0.6 * params.fSmoothness);  
#endif

#if g_FogEnable == 0
	params.cFinal.rgb = ( params.cFinal.rgb * saturate( lightDiffuseCol * ActorParams.x + derived_scene_colour.rgb * ActorParams.y ) + lightSpecularCol ) * ActorParams.z;
#elif g_FogEnable == 1 
	params.cFinal.rgb = params.cFinal.rgb * ( lightDiffuseCol + derived_scene_colour.rgb ) + lightSpecularCol;
#elif g_FogEnable == 2
  params.cFinal.rgb = ( params.cFinal.rgb*0.5 + ( surface_diffuse_colour.rgb * ActorParams.y ) + lightSpecularCol) * ActorParams.z;
#endif
			
	#endif
#endif
}

#endif

#endif