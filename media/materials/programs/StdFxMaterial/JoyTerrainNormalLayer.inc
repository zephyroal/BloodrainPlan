#ifndef _NormalLayer_INC_
#define _NormalLayer_INC_

void Normal_Layer( inout FragParams params )
{

	#if g_NormalLayerEnable == 1 && g_UpperLayerEnable == 1 && g_SM3 == 1
		float3 tn = normalize( params.FI.vTangent ).xyz;
		float3 bn = normalize( params.FI.vBinormal ).xyz;
		float3 n = normalize(params.FI.vNormal).xyz;
		float nf = params.fy;
		float3 bump = 2 * ( tex2D(NormalLayerSampler,params.FI.extTC3.zw) - float3(0.5,0.5,0.5) );

		//float3x3 rotation = float3x3(
												//tn,
												//bn,
												//n);
		//float3 normal = mul(rotation, bump);
		params.cNormalRT = float4( normalize(n + bump.x*tn*nf + bump.y*bn*nf), 1);
		
	#else
	
		params.cNormalRT = float4( normalize(params.FI.vNormal.xyz), 1 );
		
	#endif
}

#endif