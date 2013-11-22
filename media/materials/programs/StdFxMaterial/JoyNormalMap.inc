#ifndef _JoyNormalMap_INC_
#define _JoyNormalMap_INC_

void Bump_Normal( inout FragParams params )
{
#ifdef _3DSMAX_
	if ( g_NormalEnable )
	{
		float3 tn = normalize( params.FI.vTangent ).xyz;
		float3 bn = normalize( params.FI.vBinormal ).xyz;
		float3 bump = 2 * normalBumpPower * ( tex2D(NormalSampler,params.FI.baseTC) - float3(0.5,0.5,0.5) );
		params.cNormalRT = float4( normalize( normalize(params.FI.vNormal.xyz) + bump.x*tn + bump.y*bn ), 1 );
	}
	else
	{
		params.cNormalRT = float4( normalize(params.FI.vNormal.xyz), 1 );
	}
#else
	#if g_NormalMapEnable == 1
			float3 tn = normalize( params.FI.vTangent ).xyz;
			float3 bn = normalize( params.FI.vBinormal ).xyz;
			float3 bump = 2 * normalBumpPower * ( tex2D(NormalSampler,params.FI.baseTC) - float3(0.5,0.5,0.5) );
			params.cNormalRT = float4( normalize( normalize(params.FI.vNormal.xyz) + bump.x*tn + bump.y*bn ), 1 );
	#else
			params.cNormalRT = float4( normalize(params.FI.vNormal.xyz), 1 );
	#endif
#endif
}

#endif