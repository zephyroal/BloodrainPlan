uniform float4x4 worldViewProj;

void main(
	float4 Position : POSITION
	,out float4 oPosition : POSITION
	,out float2 oUV 		  : TEXCOORD0
	)
{
  oPosition = mul(worldViewProj, Position);

  // The input positions adjusted by texel offsets, so clean up inaccuracies
  float2 signPos = sign(Position.xy);
	
  // Convert to image-space
  oUV = (float2(signPos.x, -signPos.y) + 1.0f) * 0.5f;
}