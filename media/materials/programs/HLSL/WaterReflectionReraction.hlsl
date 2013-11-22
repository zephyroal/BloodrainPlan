
////////////////////////////////////////////////////////////////////////
// render fog under water
////////////////////////////////////////////////////////////////////////
void WaterFog_VS(
		float4 position	: POSITION,

		uniform float4x4 worldMatrix,
		uniform float4x4 worldViewProjMatrix,
		uniform float4 eyePosition,
		uniform float4 camFrontDir,

		out float4 oPos : POSITION,
		out float4 oUV : TEXCOORD0,
		out float4 oEyeDir : TEXCOORD1,
		out float4 oCamFrontDir : TEXCOORD2)
{
	oPos = mul(worldViewProjMatrix, position);
	float4x4 scalemat = float4x4(0.5,   0,   0, 0.5, 
	                              0,-0.5,   0, 0.5,
								   							0,   0, 0.5, 0.5,
								   							0,   0,   0,   1);
	oUV = mul(scalemat, oPos);
	oEyeDir = eyePosition - position;
	oCamFrontDir = mul(worldMatrix, camFrontDir);
}

void WaterFog_PS(
		float4 UV : TEXCOORD0,
		float4 eyeDir : TEXCOORD1,
		float4 camFrontDir : TEXCOORD2,
		uniform sampler2D depthMap : register(s0),
		
		uniform float4 FogColorDensity,
		uniform float4 viewportSize,
		out float4 oColor	: COLOR)
{
	// cal screen uv
	float2 originUV = UV.xy / UV.w + 0.5 * viewportSize.z;
	//camFrontDir = normalize(camFrontDir);
	
	float sceneDepth = -tex2D(depthMap, originUV.xy).w;
	float volumeDepth = min( dot( eyeDir, -camFrontDir ) + sceneDepth, 0 );
  //float waterVolumeFog = exp2( -FogColorDensity.w * volumeDepth / dot( normalize( eyeDir ), -camFrontDir ) );
	//float waterVolumeFog = saturate(volumeDepth * DepthLimintInv);
	
	float waterVolumeFog = 1.0 - exp(volumeDepth* FogColorDensity.w*0.00001);
	//oColor.xyz = (1.0 - (depth - 1.0) * (depth - 1.0)) * FogColorDensity.xyz * FogColorDensity.w;
	oColor.xyz = FogColorDensity.xyz;
	oColor.w = waterVolumeFog;	
	
}



////////////////////////////////////////////////////////////////////////
// render surface of water
////////////////////////////////////////////////////////////////////////
void WaterReflectionReraction_VS(
		float4 position			: POSITION,
		float4 normal		: NORMAL,
		float2 tex			: TEXCOORD0,
		float2 tex2     : TEXCOORD2,
		
		out float4 oPos	: POSITION,
    out float4 oUV		: TEXCOORD0,
	  //out float3 oNormal : TEXCOORD1,
		out float4 oEyeDir : TEXCOORD1,
		out float4 oWave0 : TEXCOORD2,
		out float4 oWave1 : TEXCOORD3,
		out float2 oUV2   : TEXCOORD4,
		
		uniform float4x4 worldViewProjMatrix,
		uniform float4x4 worldViewMatrix,
		uniform float3 eyePosition,
		uniform float timeVal,
		uniform float wavesSpeed)
{
	oPos = mul(worldViewProjMatrix, position);
	float4 pv = mul( worldViewMatrix, float4(position.xyz,1) );
	
	// cal wave UV
	float2 vTranslation= float2(0, timeVal * wavesSpeed);
  
	float4 vTex = float4( tex.xy, 0, 1 );
	oWave0.xy = vTex.xy*2*1.0 + vTranslation*2.0;
  oWave0.wz = vTex.xy*1.0 + vTranslation*3.0;
  oWave1.xy = vTex.xy*2*2.0 + vTranslation*2.0;
  oWave1.wz = vTex.xy*2*4.0 + vTranslation*3.0;
  
	// Noise map coords
	//oNoiseCoord = tex + timeVal;
	
	//oNormal = normal.xyz;
	oEyeDir.xyz = eyePosition - position.xyz;
	oEyeDir.w = -pv.z;
	
	float4x4 scalemat = float4x4(0.5,   0,   0, 0.5, 
	                              0,-0.5,   0, 0.5,
								   							0,   0, 0.5, 0.5,
								   							0,   0,   0,   1);
	oUV = mul(scalemat, oPos);
	
	oUV2 = tex2;
}


void WaterReflectionReraction_PS(
    float4 UV		: TEXCOORD0,
	  //float3 normal : TEXCOORD1,
		float4 eyeDir : TEXCOORD1,
		float4 wave0 : TEXCOORD2,
		float4 wave1 : TEXCOORD3,
		float2 oUV2  : TEXCOORD4,
		
		out float4 oColour	: COLOR,
		
		uniform sampler2D noiseMap : register(s0),
		uniform samplerCUBE reflectMap : register(s1),
		uniform sampler2D refractMap : register(s2),
		uniform sampler2D LiquidTexture : register(s3),
		//uniform sampler2D depthMap : register(s3),
		//uniform sampler2D LogicModelMaskTex : register(s4),
		
		//uniform float4 viewportSize,
		uniform float3 SunLightDir,
		uniform float4 SunColor,
		
		uniform float BigWavesScale,
		uniform float SmallWavesScale,
		uniform float2 BumpScale,
		uniform float ReflectionAmount,
		uniform float TransparencyRatio,
		uniform float SunShinePow,
		uniform float SunMultiplier,
		uniform float FresnelBias,
		uniform float2 UVScale,
		uniform float4 viewportSize
		
		//uniform float noiseScale, 
		//uniform float fresnelPower,
		//uniform float4 tintColor,
		//uniform float reflectionWeight,
		//uniform float refractionWeight
		)
{
	eyeDir.xyz = normalize(eyeDir.xyz);
	// cal screen uv
	float2 originUV = UV.xy / UV.w + 0.5 * viewportSize.zw;

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
	
	// get Reflection color
	float3 reflectDir = (2*dot(eyeDir.xyz, bumpNormal)*bumpNormal - eyeDir.xyz);
	//float3 temp = normalize(float3(reflectDir.x, -reflectDir.z, reflectDir.y+1.0f));
	float4 reflectionColour = texCUBE(reflectMap, reflectDir);
	
	// get refraction color
  float4 oriColour = tex2D(refractMap, originUV);
  float2 newUV = originUV + bumpNormal.xz*0.1;
	float4 refractionColour = tex2D(refractMap, newUV);
	//float4 LogicModelMaskColour = tex2D(LogicModelMaskTex,  newUV);
	
	if( refractionColour.w > 0.0001 || oriColour.w > 0.0001 )
		  refractionColour = oriColour;
	
	// cal fresnel	
	float vdn = abs(dot(eyeDir.xyz, bumpNormal));
	float fresnel = saturate(FresnelBias + (1-FresnelBias)*pow(1 - vdn, 5));
	
	// Final colour
	oColour = lerp(refractionColour, reflectionColour * ReflectionAmount, fresnel * TransparencyRatio);		
	float4 lightmapColor = tex2D(LiquidTexture ,oUV2 + bumpNormal.xz*0.1);		
	//if( dot(lightmapColor.xyz, lightmapColor.xyz) < float(0.001) )
  //{
  //  lightmapColor.xyz = float3(1.0, 1.0, 1.0);  
  //}
	// use sun shine
	float RdoTL = saturate(dot(reflectDir, -SunLightDir));
  float sunSpecular = pow( RdoTL , SunShinePow ); 
  float3 vSunGlow = sunSpecular * SunColor.xyz * SunMultiplier * lightmapColor.xyz ;  
 

  // add sun shine term
  oColour.xyz += vSunGlow; 
  oColour.w = 1;
}



