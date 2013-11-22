void HSVAjust_vp( float4 worldPos      : POSITION,
                  float2 textCoordi : TEXCOORD0,
                  out float4 viewPos   : POSITION,
                  out float2 textCoordo : TEXCOORD0,
                  uniform float4x4 worldViewProj
                 )       
{
    viewPos = mul(worldViewProj, worldPos);
    textCoordo = textCoordi;
}

void HSVAjust_fp( float2 tex  : TEXCOORD0,
                  out float4 color : COLOR,
                  uniform sampler2D scene : register(s0),             // full resolution image
                  uniform float4 hsvFactor,
				  uniform float  farClipDist
                )
{
	color = tex2D(scene, tex);
	float sceneDepth = -tex2D(scene, tex).w;
	
	float depthLimit = farClipDist * hsvFactor.x;
		
	if(sceneDepth > depthLimit)
	{
		float s	= hsvFactor.y;
		float v = hsvFactor.z;
			
		float linearFactor = saturate((sceneDepth - depthLimit) / (farClipDist - depthLimit));
		float3 linearLumFactor = v * linearFactor * 0.3f;
		color.xyz += linearLumFactor;
			
		float saturate = dot(color.xyz, float3(0.2126f, 0.7152f, 0.0722f));
		float  linearSatureateFactor = s * linearFactor;
		color.xyz = lerp(saturate.xxx, color.xyz, 1 - linearSatureateFactor);
	}
				
	color.w = 1.0f;
}