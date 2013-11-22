
//#define ROWVECTOR

#ifdef ROWVECTOR
#define MtxMul(v, m) mul(v, m)
#else
#define MtxMul(v, m) mul(m, v)
#endif

#ifndef TEXTURE_USED
    #define TEXTURE_USED 1
#endif

//vertex format definition
struct VertexInFormat {
    float3 Position	: POSITION;
    float4 UV		: TEXCOORD0;
};

struct VertexOutFormat {
    float4 HPosition	: POSITION;
    float2 UV		: TEXCOORD0;
	  float2 DepthInfo : TEXCOORD1;
};

///////// VERTEX SHADING /////////////////////

VertexOutFormat cartoonDrawDepth_VS(
	VertexInFormat IN,
	
	uniform float4x4 WorldViewXf,
	uniform float4x4 WvpXf
) {
    VertexOutFormat OUT = (VertexOutFormat)0;
    float4 Po = float4(IN.Position.xyz,1);
	
	  //calculate vertex pos in world view space
	  float3 PositionInView = mul(WorldViewXf, Po).xyz; 
	  //save the liner depth rather than projective depth for more precise
	  OUT.DepthInfo.x = -PositionInView.z;	//attention for the camera direction,it point to -Z

    OUT.UV = IN.UV.xy;

    OUT.HPosition = MtxMul(Po,WvpXf); //计算齐次空间坐标
	
	  OUT.DepthInfo.y = OUT.HPosition.z / OUT.HPosition.w;
	
    return OUT;
}

///////// PIXEL SHADING //////////////////////

void cartoonDrawDepth_PS(
	VertexOutFormat IN,
	
	#if TEXTURE_USED > 0
	uniform sampler diffuseSampler,
	#endif
	
	uniform float specialFlag,
	
	out float4 outDepth : COLOR
) 
{
	#if TEXTURE_USED > 0
	    float4 diffuseColor = tex2D(diffuseSampler, IN.UV);
	#else
	    float4 diffuseColor = float4(1, 1, 1, 1);
	#endif
	float alpha = diffuseColor.w;
	//pass alpha value to the render pipeline, let the dardware clip pixels
	//clip(alpha - alphaRejection);	
	
	outDepth = float4(0, 0, specialFlag, 0);
	outDepth.xy = IN.DepthInfo.xy;
}