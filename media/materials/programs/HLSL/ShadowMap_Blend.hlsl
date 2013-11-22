//////////////////////////////////////////////////////////////////////////////
struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 oTexCoord: TEXCOORD0;
   float3 oRay : TEXCOORD1;
};

struct PS_OUTPUT {
		float4 color : COLOR;
};

static const float c_GBlurWeights[13] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};
//////////////////////////////////////////////////////////////////////////////
uniform sampler2D sceneTex : register(s0);             // full resolution image with depth values
uniform sampler2D shadowTex : register(s1);         		// shadowMap
uniform float4	halfPixel;
uniform float4	shadowParams;
//////////////////////////////////////////////////////////////////////////////
float GaussianBlur( float2 shadowUV, float2 offset)
{
  offset *= 2 * halfPixel.xy;
	float sum = 0;
	for ( int i = -6; i <= 6; ++i )
	{
		float t = tex2D( shadowTex, shadowUV + shadowParams.w * offset * i ).x;
		sum += t * c_GBlurWeights[i + 6];
	}
	return sum;
}

//////////////////////////////////////////////////////////////////////////////
PS_OUTPUT Blend_ShadowMap_fp( VS_OUTPUT IN )
{
	PS_OUTPUT Out = (PS_OUTPUT)0;
	float4 sceneColor = tex2D(sceneTex, IN.oTexCoord.xy + halfPixel.xy);
	float blurH = GaussianBlur(IN.oTexCoord.xy + halfPixel.xy , float2(1,0));
	float blurV = GaussianBlur(IN.oTexCoord.xy + halfPixel.xy , float2(0,1));
	
	float shadowColor = (blurH + blurV) * 0.5;
	
	sceneColor.xyz = sceneColor.xyz * shadowColor;
	
	Out.color = sceneColor ;
	
	return Out;
}

/*for ( i = -1.5; i < 1.5; i+= 1)
{
	for ( J = -1.5; J < 1.5; J+= 1)
	{
		tex2Dproj( sceneTex, float4( (IN.oTexCoord.xy + halfPixel.xy) * w, w, w))
	}
}*/