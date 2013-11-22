#ifndef _JoyGrass_INC_
#define _JoyGrass_INC_

//计算草的偏移值
float4 calc_wave_offset( in float4 offset, in float4 vertexPos, in float4 propertyParam )
{	
	//offset x - xz平面的x偏移值 
	//y - xz平面的x扰动位置 
	//z - xz平面的z偏移值 
	//w - xz平面的z扰动位置

	//求交互点与顶点间的距离
	float2 vectorDistance = vertexPos.xz - offset.yw;
	float distance = vectorDistance.x * vectorDistance.x + vectorDistance.y * vectorDistance.y;
	distance = sqrt(distance);
	//如果两点间的距离大于MAX_DISTANCE,则顶点不需要特殊处理
	if( distance > propertyParam.x )
	{
		return float4(offset.x, 0, offset.z, 0);
	}
	else
	{
		float sin = vectorDistance.y / distance;
		float cos = vectorDistance.x / distance;
		float moveDistance = propertyParam.x - distance;
		float interactiveOffsetX = moveDistance * cos * propertyParam.y;
		float interactiveOffsetZ = moveDistance * sin * propertyParam.z;
		offset.x += interactiveOffsetX;
		offset.z += interactiveOffsetZ;
		return float4(offset.x, 0, offset.z, 0);
	}
}
//计算草的顶点位置
float4 calc_grass_pos(in float4 grassOffset, in float4 po, in float2 uv, in float4 grassParam)
{	
	float4 pw;
	float4 p;
	float4 factor = float4(1,1,1,1) - uv.yyyy;
	pw = mul( world_matrix, float4(po.xyz,1.0) );
	p = po + factor * calc_wave_offset(grassOffset, pw, grassParam);
	return float4(p.xyz, 1.0);
}

#endif