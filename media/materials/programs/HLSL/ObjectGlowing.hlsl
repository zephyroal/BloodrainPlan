void ScreenQuadVS(
		float4 Position : POSITION, 

		out float4 outPosition : POSITION,
		out float2 outUV : TEXCOORD0,
		
		uniform float4x4 WvpXf
) {
    // Use standardise transform, so work accord with render system specific (RS depth, requires texture flipping, etc)
    outPosition = mul(WvpXf, Position);

    // The input positions adjusted by texel offsets, so clean up inaccuracies
    float2 signPos = sign(Position.xy);

    // Convert to image-space
    outUV = (float2(signPos.x, -signPos.y) + 1.0f) * 0.5f;
}
/* 
------------------------------------------------------------------
  get glow
------------------------------------------------------------------
*/
float4 GetGlow_PS(
	float2 texUV : TEXCOORD0,
	
	uniform sampler sceneColorMap,
	uniform float4 sampleOffset[8]
) : COLOR0
{
  return tex2D( sceneColorMap,texUV);
}
float4 GetGlow2_PS(
	float2 texUV : TEXCOORD0
) : COLOR0
{
  return float4(1,0,0,1);
}
/*
------------------------------------------------------------------
  down sample scene color target to half its size, 
  down sample the target for larger area flood lighting
------------------------------------------------------------------
*/
float4 DownSample_PS(
	float2 texUV : TEXCOORD0,
	
	uniform sampler sceneColorMap,
	uniform float4 sampleOffset[8]
) : COLOR0
{
  float4 average = { 0.0f, 0.0f, 0.0f, 0.0f };
  for( int i = 0; i < 8; i++ )
  {
    average += tex2D( sceneColorMap, texUV + float2(sampleOffset[i].x, sampleOffset[i].y) );
    average += tex2D( sceneColorMap, texUV + float2(sampleOffset[i].z, sampleOffset[i].w) );
  }
    
  average /= 16;

  return average;
}

/*
------------------------------------------------------------------
  horizontally blur the down sampled scene color target
------------------------------------------------------------------
*/
float4 HorizontalBlur_PS( 
	float2 texUV : TEXCOORD0,
	
	uniform sampler downSampledSceneMap,
	uniform float4 viewportSize
) : COLOR
{
	float BloomWeights[9];
    float BloomOffsets[9];
	float2 PixelSize = viewportSize.zw;
  
	BloomOffsets[0] = -4.0f * PixelSize.x;
	BloomWeights[0] = 0.2; 
	BloomOffsets[1] = -3.0f * PixelSize.x;
	BloomWeights[1] = 0.4;  
	BloomOffsets[2] = -2.0f * PixelSize.x;
	BloomWeights[2] = 0.6;  
	BloomOffsets[3] = -1.0f * PixelSize.x;
	BloomWeights[3] = 0.8; 
	BloomOffsets[4] = -0.0f * PixelSize.x;
	BloomWeights[4] = 1.0;  
	BloomOffsets[5] = 1.0f * PixelSize.x;
	BloomWeights[5] = 0.8;  
	BloomOffsets[6] = 2.0f * PixelSize.x;
	BloomWeights[6] = 0.6;  
	BloomOffsets[7] = 3.0f * PixelSize.x;
	BloomWeights[7] = 0.4; 
	BloomOffsets[8] = 4.0f * PixelSize.x;
	BloomWeights[8] = 0.2;  
	
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    float totalWeight = 0.0;
    for( int j = 0; j < 9; j++ )
    {
        color += (tex2D( downSampledSceneMap, texUV + float2( BloomOffsets[j], 0.0f ) ) * BloomWeights[j] );
        totalWeight += BloomWeights[j];
    }
    color /= totalWeight;
    return float4( color.rgb, 1.0f );
}

/*
------------------------------------------------------------------
  vertically blur the down sampled scene color target
------------------------------------------------------------------
*/
float4 VerticalBlur_PS( 
	float2 texUV : TEXCOORD0,
	
	uniform sampler horizontalBluredSceneMap,
	uniform float4 viewportSize
) : COLOR
{
	float BloomWeights[9];
    float BloomOffsets[9];
    
  float2 PixelSize = viewportSize.zw;
	
	BloomOffsets[0] = -4.0f * PixelSize.y;
	BloomWeights[0] = 0.2; 
	BloomOffsets[1] = -3.0f * PixelSize.y;
	BloomWeights[1] = 0.4;  
	BloomOffsets[2] = -2.0f * PixelSize.y;
	BloomWeights[2] = 0.6;  
	BloomOffsets[3] = -1.0f * PixelSize.y;
	BloomWeights[3] = 0.8; 
	BloomOffsets[4] = -0.0f * PixelSize.y;
	BloomWeights[4] = 1.0;  
	BloomOffsets[5] = 1.0f * PixelSize.y;
	BloomWeights[5] = 0.8;  
	BloomOffsets[6] = 2.0f * PixelSize.y;
	BloomWeights[6] = 0.6;  
	BloomOffsets[7] = 3.0f * PixelSize.y;
	BloomWeights[7] = 0.4; 
	BloomOffsets[8] = 4.0f * PixelSize.y;
	BloomWeights[8] = 0.2;  
	
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    float totalWeight = 0.0;
    for( int j = 0; j < 9; j++ )
    {
        color += (tex2D( horizontalBluredSceneMap, texUV + float2( 0.0f, BloomOffsets[j] ) ) * BloomWeights[j] );
        totalWeight += BloomWeights[j];
    }
    color /= totalWeight;
    return float4( color.rgb, 1.0f );
}

float4 blend_fp(
        in float2 baseTexCoord : TEXCOORD0,
        uniform float lightWeight,
        uniform sampler glowImage,
        uniform sampler baseImage,
        uniform sampler blurImage
        ) : COLOR
{
		float4 glowColor = tex2D(glowImage, baseTexCoord);
    float4 baseColor = tex2D(baseImage, baseTexCoord);
    float4 blurColor = tex2D(blurImage, baseTexCoord);
    float4 outputColor = float4(1.0, 1.0, 1.0, 1.0);
    outputColor.xyz = baseColor.xyz + glowColor.xyz*lightWeight + blurColor.xyz*2*lightWeight;

    return outputColor;
}