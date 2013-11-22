
void godrayMask_PS(
    float2 texCoord : TexCoord0,
    out float4 color : COLOR,
    uniform sampler2D SceneTex : register(s0),
    uniform sampler2D maskTex  : register(s1),   
    uniform sampler2D EnSkyTex : register(s2)   
)
{
		float alpha = tex2D(maskTex, texCoord).w;
		//alpha = 0.1 ：背景天空
		//alpha = 1.0 : 地表，建筑，云等遮挡物
		// 0.1 < alpha < 1.0 : 太阳
    if( alpha < 0.15 )
    {
       //coord of sample (U, V) = (0.5, alpha)
       //float2 sampleUV = float2(0.5, 0.6);
       color.rgb =  0.25 * tex2D(EnSkyTex, float2(0.5, 0.5)).rgb;
       color.a   =  1.0;
    }
    else
    {
      color.rgb = saturate(1.0 - alpha) * tex2D(SceneTex, texCoord).rgb;
      color.a = 1.0;
    } 
}


void godray_PS(
    float2  texCoord : TexCoord0,
    uniform float4 viewportSize,
    uniform float Density, 
    uniform int NUM_SAMPLES,
    uniform float fNUM_SAMPLES,  
    uniform float Weight,
    uniform float Exposure,
    uniform float Decay,
    uniform float4 ScreenLightPos,
    out float4 finalColor : COLOR,
    uniform sampler2D sceneTex : register(s0),
    uniform sampler2D GodRayMaskTex  : register(s1)
)
{   
  texCoord = texCoord + 0.5 * viewportSize.zw;
  float4 sceneColor = tex2D(sceneTex, texCoord); 
  if( ScreenLightPos.z > 0.0001 )
  {    
  float2 deltaTexCoord = (texCoord - ScreenLightPos.xy);  
  deltaTexCoord *= 1.0f /fNUM_SAMPLES * Density; 
  float3 color = tex2D(GodRayMaskTex, texCoord);
  float illuminationDecay = 1.0f; 
  for (int i = 0; i < 64; i++)
  {
    texCoord -= deltaTexCoord;
    float3 sample = tex2D(GodRayMaskTex, texCoord);
    sample *= illuminationDecay * Weight;
    color += sample;
    illuminationDecay *= Decay;
  } 
  
  float blendWeight = 0.1;
  finalColor = float4(color * Exposure, 0.0) * blendWeight + sceneColor * (1-blendWeight);
  } 
  else
  {
   finalColor = sceneColor;  
  } 
}