//////////////////////////////////////////////////////////////////////////////
struct VS_OUTPUT {
		float4 Pos: POSITION;
		float2 oTexCoord: TEXCOORD0;
		float3 oRay : TEXCOORD1;
};

struct PS_OUTPUT {
		float4 color: COLOR;
};

//////////////////////////////////////////////////////////////////////////////
uniform float4x4  cWorldVecGen;

VS_OUTPUT Receive_ShadowMap_vp( in float4 Pos: POSITION	)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	// Clean up inaccuracies
	Pos.xy = sign(Pos.xy);

	Out.Pos = float4(Pos.xy, 1, 1);
	
	// Image-space
	Out.oTexCoord.x = 0.5 * (1 + Pos.x);
	Out.oTexCoord.y = 0.5 * (1 - Pos.y);

  float4 worldVec = mul(cWorldVecGen, Out.Pos.xyww);
  Out.oRay = worldVec.xyz / worldVec.w; 
  return Out;
}

//////////////////////////////////////////////////////////////////////////////
uniform sampler2D sceneTex : register(s0);			// full resolution image with depth values
uniform sampler2D shadowTex : register(s1);			// shadowMap
uniform float4 cCamZVecInWorld;
uniform float3 vfViewPos;
uniform float4x4 ShadowViewProj;
uniform float4	halfPixel;
uniform float4	shadowParams;

#define MAX_DEPTH 3000000

//////////////////////////////////////////////////////////////////////////////
float getPCFShadow(	sampler2D shadowMapDepth, float2 shadowUV, float depth, float kernal)
{
	float shadowFactor = 0;
	float2 offset = halfPixel.zw;  
	for (int i = 0 ; i < kernal ; ++i)
	{
		float x = i - kernal/2;
		for( int j = 0 ; j < kernal ; ++j)
		{
			float y = j - kernal/2;
			float sampleDepth = tex2D(shadowMapDepth, shadowUV+float2(x,y)*offset).x ;
			sampleDepth = (abs(sampleDepth) < 0.001) ? MAX_DEPTH : sampleDepth;
			shadowFactor += (( depth - sampleDepth) > shadowParams.x )? shadowParams.y : 0.0;
		}
	}
	
	shadowFactor /= (kernal*kernal);
	float distance = max(1 - length(shadowUV - float2(0.5,0.5)) * 2, 0);
	
	shadowFactor *= min( distance/(1.0001 - shadowParams.z), 1);
		
	return 1 - shadowFactor;
}

//////////////////////////////////////////////////////////////////////////////
void calculate_world_pos(in float sceneDepth, in float3 ray, out float3 worldPos)
{	  
  float3 worldVec;			// 世界空间下到摄像机到物体向量（只记录方向）
  worldVec = normalize(ray);
  // 获得摄像机Z负方向（在世界空间中）和摄像机到物体向量夹角的余弦，用于计算该物体的实际深度
  float cosTheta = dot(worldVec, normalize(cCamZVecInWorld.xyz));
  
  // 计算世界空间下摄像机到物体的实际向量(当前深度不是实际距离，要除以cosTheta)
  float3 cameraToWorldPos = sceneDepth / cosTheta * worldVec;

  // 计算物体在世界空间中的坐标
  worldPos = cameraToWorldPos + vfViewPos;
}

//////////////////////////////////////////////////////////////////////////////
float3 encodeRGB8_16( in float3 iColorLow, in float3 iColorHigh )
{
		return floor(iColorLow*255)+(min(iColorHigh, 0.99));
}

//////////////////////////////////////////////////////////////////////////////
void decodeRGB16_8( in float3 iColor, out float3 oColorLow, out float3 oColorHi )
{
		float3 tmp = floor( iColor );
		oColorLow = tmp / 255.f;
		oColorHi = iColor - tmp ;
}

//////////////////////////////////////////////////////////////////////////////
PS_OUTPUT Receive_ShadowMap_fp( VS_OUTPUT IN )
{
	PS_OUTPUT Out = (PS_OUTPUT)0;
	
	float4 sceneColor = tex2D(sceneTex, IN.oTexCoord.xy + halfPixel.xy);
	Out.color = sceneColor;
	
	if(sceneColor.w < 1.0)
		return Out;
		
	float3 worldPos;
	calculate_world_pos(sceneColor.w, IN.oRay.xyz, worldPos );
	
	float4 shadowPos = mul(ShadowViewProj, float4(worldPos, 1));
	
	float2 shadowUV = shadowPos.xy / shadowPos.w;
	
	if(abs(shadowUV.x) > 0.99 || abs(shadowUV.y) > 0.99)
		return Out;
	
	shadowUV = float2(0.5, 0.5) + float2(0.5, -0.5) * shadowUV + halfPixel.zw * 0.5;
	
	float sampleDepth = tex2D(shadowTex, shadowUV.xy).x;
	if ( sampleDepth > 0.999 )
	{
		return Out;
	}
	
	float shadowFactor = getPCFShadow( shadowTex, shadowUV, shadowPos.z, 3.0);
	Out.color.xyz *= shadowFactor;
	
	return Out;
}