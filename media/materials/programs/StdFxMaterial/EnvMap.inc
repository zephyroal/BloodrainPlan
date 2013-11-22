

bool g_ReflectionMaskEnable <
	string UIName = "Reflection Mask Enable";
> = false;



texture g_ReflectionMaskTexture < 
	string UIName = "Reflection Mask";
	//int Texcoord = 0;
	//int MapChannel = 1;
>;

float4 calcReflection(float3 N,float3 V,float2 mask_uv)
{
	float4 env =float4(0,0,0,0);
	if(g_ReflectionEnable)
	{
#ifdef _3DSMAX_
		float3 CUV = reflect(V, N).xzy;
		CUV.y = -CUV.y;
#else
		float3 CUV = reflect(V, N).xyz;
		CUV.y = -CUV.y;
#endif
		env = texCUBE(g_ReflectionSampler, CUV);
		if(g_ReflectionMaskEnable)
		{
			float reflection_level=tex2D(g_ReflectionMaskSampler,mask_uv).x;
			float v1=reflection_level*g_ReflectionAmount;
			env *= v1;
		}
		else
		{
			env *=g_ReflectionAmount;
		}
	}
	return env;
}

//for Deferred shading
float4 calcReflection_DL(float3 N,float3 V,float reflection)
{
	float3 CUV = normalize(reflect( V, N).xzy);
	CUV.y = -CUV.y;
	float4 env = texCUBE(g_ReflectionSampler, CUV);
	env*=reflection;
	return env;
}

