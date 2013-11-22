
float3 normal_calc(float3 mapNorm)
{
#ifdef _3DSMAX_
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	mapNorm.y=mapNorm.y;
#else
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	mapNorm.y=mapNorm.y;
#endif
	return mapNorm;
}

const float4x4 identity_matrix=float4x4
(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1 
);
