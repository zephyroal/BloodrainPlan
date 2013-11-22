struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
};

VS_OUTPUT vsNearCombine(
	 float4 Pos: POSITION
	,uniform float4 viewportSize
)
{
   VS_OUTPUT Out;

	// Clean up inaccuracies
	Pos.xy = sign(Pos.xy);

	Out.Pos = float4(Pos.xy, 0, 1);
	
	// Image-space
	Out.texCoord.x = 0.5 * (1 + Pos.x)  + viewportSize.zw * 0.5;
	Out.texCoord.y = 0.5 * (1 - Pos.y)  + viewportSize.zw * 0.5;

   
	return Out;
}



float4 psNearCombine(
  float2 texCoord: TEXCOORD0
, uniform sampler Tex0: register(s0)   // source
, uniform sampler Tex1: register(s1)   
)  : COLOR 
{
    float4 a0 = tex2D(Tex0, texCoord); // fog color
    float4 a1 = tex2D(Tex1, texCoord); // scene color
    
    float4 color;
    color.xyz = a0.xyz* a0.w  + a1.xyz*(1-a0.w); // 

 
    color.w = 1;
     return color;
}

