void std_quad_vs(
        float4 pos : POSITION,
        float2 texCoord : TEXCOORD0,

        out float4 oPos : POSITION,
        out float2 oTexCoord : TEXCOORD0,

        uniform float4x4 worldViewProj
        )
{
    oPos = mul(worldViewProj, pos);

    oTexCoord = texCoord;
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