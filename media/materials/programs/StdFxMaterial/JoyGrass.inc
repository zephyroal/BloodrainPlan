#ifndef _JoyGrass_INC_
#define _JoyGrass_INC_

//����ݵ�ƫ��ֵ
float4 calc_wave_offset( in float4 offset, in float4 vertexPos, in float4 propertyParam )
{	
	//offset x - xzƽ���xƫ��ֵ 
	//y - xzƽ���x�Ŷ�λ�� 
	//z - xzƽ���zƫ��ֵ 
	//w - xzƽ���z�Ŷ�λ��

	//�󽻻����붥���ľ���
	float2 vectorDistance = vertexPos.xz - offset.yw;
	float distance = vectorDistance.x * vectorDistance.x + vectorDistance.y * vectorDistance.y;
	distance = sqrt(distance);
	//��������ľ������MAX_DISTANCE,�򶥵㲻��Ҫ���⴦��
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
//����ݵĶ���λ��
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