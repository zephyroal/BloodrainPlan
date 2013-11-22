uniform float4x4 worldViewProj;

void main(
	 float4 Pos: POSITION
	,float2 UV : TEXCOORD0
	,uniform     float      timePast
	,out float4  oPosition  : POSITION
	,out float4  oObjPos		: TEXCOORD1
	,out float4  oUV  			: TEXCOORD2
	)
{
   oPosition = mul(worldViewProj, Pos);
   oObjPos	 = Pos;
	 oUV	  	 = float4(UV, timePast, 0);
}