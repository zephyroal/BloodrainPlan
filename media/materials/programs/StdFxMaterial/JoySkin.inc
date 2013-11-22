#ifndef _JoySkin_INC_
#define _JoySkin_INC_

/// calc hw skin
/// inputs 	: po
/// outputs	: pw
void Calc_Skin( inout VertParams params )
{
	// tmp
	float4x4 WorldMat = world_matrix;
	
#if g_UseSkin == 1

	float3 blendPos = float3(0,0,0);
	float3 blendNormal = float3(0,0,0);
	
	float4x4 finalMatrix;
	for( int i = 0; i < g_BoneNum.x; i++ )
	{
			finalMatrix[0] = world_matrix_array_3x4[params.VI.blendIdx[i]][0];
			finalMatrix[1] = world_matrix_array_3x4[params.VI.blendIdx[i]][1];
			finalMatrix[2] = world_matrix_array_3x4[params.VI.blendIdx[i]][2];
			finalMatrix[3] = float4(0,0,0,1);
			finalMatrix = MUL( world_matrix, finalMatrix );	
			
			blendPos +=  (MUL( finalMatrix, params.VI.position ).xyz ) * params.VI.weight[i];
			blendNormal += MUL( finalMatrix, float4(params.VI.normal.rgb,0) ) * params.VI.weight[i] ;
	}
	
	params.pw = float4( blendPos.xyz, 1 );
	params.vNormal = float4( blendNormal.xyz, 0 );
	
#else
	
		params.pw = MUL( world_matrix, params.VI.position );
		params.vNormal = MUL( world_matrix, float4(params.VI.normal.xyz, 0) );
	
	// even inverse_transpose_world_matrix should assign
	// world_matrix do rigth in most cases
	//params.vNormal = MUL( world_matrix, float4(params.VI.normal.xyz, 0) );
	
#if g_GrassEnable == 1
	params.vNormal = MUL( WorldMat, params.VI.normal);
#endif
	
#endif

#if g_IsPlants == 1
	params.vNormal = MUL( WorldMat, float4(params.VI.position.xyz,0) );
#endif

#if g_GrassEnable != 0
  params.vNormal.w = params.VI.normal.w;
#endif

}

#endif