//体积雾反面深度

void vsVolumeCWDepth
(
      float4 position       : POSITION
 ,out float4 oPosition      : POSITION
 ,out float4 oObjPosition   : TEXCOORD0
 ,uniform    float4x4       wvpMat
)
{
	oPosition    = mul(wvpMat, position);
	oObjPosition = position;
}

void psVolumeCWDepth 
(
   float4  objPosition : TEXCOORD0
  ,out     float4      oDepth      : COLOR
  ,uniform float4x4    worldviewMat
) 
{	
	float4 viewPosition = mul(worldviewMat, objPosition);
	
	oDepth = -viewPosition.z;
}