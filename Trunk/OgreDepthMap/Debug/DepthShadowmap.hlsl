// vertex shader in DepthShadowmap.hlsl
void casterVP(
   float4 position     : POSITION,
   out float4 outPos   : POSITION,
   out float2 outDepth : TEXCOORD0,

   uniform float4x4 worldViewProj,
   uniform float4 texelOffsets)
{
   outPos = mul(worldViewProj, position);
   outPos.xy += texelOffsets.zw * outPos.w;
   outDepth.x = outPos.z;
   outDepth.y = outPos.w;
}

// pixel shader in DepthShadowmap.hlsl
void casterFP(
   float2 depth      : TEXCOORD0,
   out float4 result : COLOR)
   
{
   float finalDepth = depth.x;
   result = float4(finalDepth, 0, 0, 1);
}

