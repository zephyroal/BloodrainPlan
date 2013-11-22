

void MagmaFluid_VS(
    float4 position	: POSITION,
		float4 normal		: NORMAL,
		float2 tex			: TEXCOORD0,
		float2 tex1     : TEXCOORD1,
		
		out float4 oPos	: POSITION,
		out float4 oUV	: TEXCOORD0,
		out float4 oWave0 : TEXCOORD1,
		out float4 oWave1 : TEXCOORD2,
		out float2 oUV1   : TEXCOORD3,
		
		uniform float4x4 worldViewProjMatrix,
		uniform float  timeVal,
		uniform float wavesSpeed)
{
   oPos = mul(worldViewProjMatrix, position);
	
	// cal wave UV
	float2 vTranslation= float2(0, timeVal * wavesSpeed);
  
	float4 vTex = float4( tex.xy, 0, 1 );
	oWave0.xy = vTex.xy*2*1.0 + vTranslation*2.0;
  oWave0.wz = vTex.xy*1.0 + vTranslation*3.0;
  oWave1.xy = vTex.xy*2*2.0 + vTranslation*2.0;
  oWave1.wz = vTex.xy*2*4.0 + vTranslation*3.0;

	oUV = float4(vTex.xy, vTex.xy + float2(vTranslation.y*0.5, vTranslation.y));	
	oUV1 = tex1;
}

void MagmaFluid_PS(
    float4 UV		: TEXCOORD0,
		float4 wave0 : TEXCOORD1,
		float4 wave1 : TEXCOORD2,
		float2 oUV1  : TEXCOORD3,
				
		out float4 oColour	: COLOR,
				
		uniform sampler2D noiseMap : register(s0),
		uniform sampler2D refractMap : register(s1),
		uniform sampler2D depthMap   : register(s2),

		uniform float BigWavesScale,
		uniform float SmallWavesScale,
		uniform float2 BumpScale,
		uniform float2 UVScale,
		uniform float2 ImageSampleValue)
{
	//eyeDir = normalize(eyeDir);
	float2 originUV = UV.xy;

	// cal normal
	float3 bumpColorA = half3(0,1,0);
  float3 bumpColorB = half3(0,1,0);
  float3 bumpColorC = half3(0,1,0);
  float3 bumpColorD = half3(0,1,0);
  float3 bumpLowFreq = half3(0,1,0);
  float3 bumpHighFreq = half3(0,1,0);
    
  // merge big waves
  bumpColorA.xz = tex2D(noiseMap, wave0.xy*UVScale).xy;           
  bumpColorB.xz = tex2D(noiseMap, wave0.wz*UVScale).xy;           
  bumpLowFreq.xz = (bumpColorA.xz + bumpColorB.xz)*BigWavesScale - BigWavesScale;

  // merge small waves
  bumpColorC.xz = tex2D(noiseMap, wave1.xy*UVScale).xy;
  bumpColorD.xz = tex2D(noiseMap, wave1.wz*UVScale).xy;
  bumpHighFreq.xz = (bumpColorC.xz + bumpColorD.xz)*SmallWavesScale - SmallWavesScale;

  // merge all waves
  float3 bumpNormal = float3(0,1,0);
  bumpNormal.xz = bumpLowFreq.xz + bumpHighFreq.xz;

  bumpNormal.xyz = normalize( bumpNormal.xyz );
	bumpNormal.xz *= BumpScale;
	bumpNormal.xyz = normalize( bumpNormal.xyz );	
	
  float2 newUV = originUV + bumpNormal.xz*0.1 + UV.zw * ImageSampleValue.xy;
	float4 refractionColour = tex2D(refractMap, newUV);
	
	float4 depthColor = tex2D(depthMap, oUV1);
			
	// Final colour
	oColour = refractionColour;
  oColour.w = depthColor.w;
}