#ifndef _JoyLightUtility_INC_
#define _JoyLightUtility_INC_

/// gamma correction
float4 Gamma( float4 c, FragParams params )
{		
#if 0
		return float4( pow( c.rgb, params.fGammaVal ), c.a );
#else
		return c;
#endif
		
}

/// hdr scale
float4 HDROutput( FragParams params )
{
#if 0
		// gamma correction
		params.fGammaVal = 1 / params.fGammaVal;
		if ( params.Opt.bHDREnable )
		{
				return float4( Gamma(params.cFinal,params).rgb * params.fHDRPower, params.cFinal.a );
		}
		else
		{
				return params.cFinal;
		}
#else
		return params.cFinal;
#endif
}

/// format render option
void fmt_rop( inout FragParams params )
{
		params.fGammaVal					= EditorParams.y;
		params.Opt.bWhiteScene		= 0; // EditorParams.x;
		params.Opt.bHDREnable			= EditorParams.z;
		params.Opt.nShadingQuality= EditorParams.w;
		params.fHDRPower					= HDRParam.x;
		
		params.fSmoothness				= g_LightingParams.x;
		params.fGlossiness				= g_LightingParams.y;
		params.fShiness						= 128; // g_LightingParams.z;
}

#define blackOffset 0.15f
#define whiteColor  1.0f
#define halfColor   0.5f
#define blackColor  0.0f
float3 OverlayBlend( float3 baseColor, float3 colorMapColor )
{
		colorMapColor = clamp( colorMapColor, blackOffset, whiteColor ) ;
		float3 factor = clamp( sign(baseColor - halfColor), blackColor, whiteColor );
		float3 result = factor + 2 * (1-2*factor) * (factor - colorMapColor) * (factor - baseColor);
		
		return result;
}

float4 HDRScale( float4 c, float scaler )
{
		return float4( c.rgb * scaler, c.a );
}

float desaturate( float3 color )
{
		return dot( color,float3(0.299,0.587,0.114) );
}

float fresnel( float3 normal, float3 eyeVec, float power, float bias )
{
		float fresnel = saturate( abs(dot(normal,eyeVec)) );
		fresnel = 1 - fresnel;										
		fresnel = pow( fresnel, power );
		fresnel += bias;
		return saturate(fresnel);
}

float4 getEnvirionmentCMap_lod( samplerCUBE envMap, in float3 envTC, in float fNdotE, in float fSpecPower )
{
		float fEdgeMipFix = saturate( 1 - fNdotE );
		fEdgeMipFix = fEdgeMipFix * fEdgeMipFix * fEdgeMipFix;
		
		// half fGlossinessLod = 8.h + log2(1.h-exp2(-1.h/fSpecPower));
		float fGlossinessLod = 6.16231h - 0.497418h * sqrt(fSpecPower);
		float4 envColor = texCUBElod( envMap, float4(envTC, fGlossinessLod) );
		return envColor;
}

float4 getEnvirionmentCMap( samplerCUBE envMap, in float3 envTC, in float fSpecPower )
{
		// half fGlossinessLod = 8.h + log2(1.h-exp2(-1.h/fSpecPower));
		// half fGlossinessLod = 6.16231h - 0.497418h * sqrt(fSpecPower);
		// 暂时直接使用lod采样
		float4 envColor = texCUBElod( envMap, float4(envTC, g_ReflectionAmount) );
		return envColor;
}

float4 getEnvirionment2DMap( sampler2D envMap, in float2 envTC )
{
		float4 envColor = tex2D( envMap, envTC.xy );
		return envColor;
}

// wrap = 0.6, scatterWidth = 0.8, scatterColor = {0,0.15,0,0}
float4 scatter( float wrap, float scatterWidth, float4 scatterColor, float n, float l )
{
		float NdotL_wrap = ( dot(n,l) + wrap ) / ( 1 + wrap );
		float4 scatter = smoothstep( 0, scatterWidth, NdotL_wrap ) * smoothstep( scatterWidth * 2, scatterWidth, NdotL_wrap );
		return scatter;
}

#endif