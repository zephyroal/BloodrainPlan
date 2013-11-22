// Screen Space Ambient Occlusion shaders
// Ambient Occlusion Map pass

// uniform samplers
sampler2D DepthMap			: register(s0);
sampler2D Screen				: register(s1);
sampler2D Normal				: register(s2);

#include "JoyCodec.inc"

float4 SSAO_Blur_PS	( 	float2 intex :TEXCOORD0,
						uniform float dir,
						uniform float4 inverse_texture_size
					) : COLOR0
{

	float color 	= tex2Dlod( Screen, 	float4(intex,0,0)).r;
	float2 tmpNormal = tex2Dlod( Normal, 	float4(intex,0,0)).zw;
	float3 norm 	= normalize( decode_normal(tmpNormal) );;
	float depth 	= tex2Dlod( DepthMap, 	float4(intex,0,0)).w;

	float num 		= 1;
	int blurdist 	= 12;
	
	float2 blurdir;
	if(dir == 0)
	{
		blurdir = float2(inverse_texture_size.x,0);
	}	
	else
	{
		blurdir = float2(0,inverse_texture_size.y);
	}
		
	for( int i = -blurdist/2; i <= blurdist/2; i+=2)
	{
		float4 texCoord 	= float4(intex + i * blurdir.xy, 0, 0 );
		
		float newsample 	= tex2Dlod( Screen,		texCoord ).r;	
		float3 newnormal 	= tex2Dlod( Normal,		texCoord );
		float newdepth 	= tex2Dlod( DepthMap, texCoord ).x;


		float depthDif 		= newdepth - depth;
		float depthDifSqr = depthDif* depthDif;
				
		// Blur if samples are less than 10cm apart	and angle between their normals is less than about 10 deg
		if( ( dot( newnormal, norm) > 0.99 ) && ( depthDifSqr < 100.0 ) )
		{
			num +=   ( blurdist/2 - abs(i));
			color += newsample *  ( blurdist/2 - abs(i));
		}
	}

	// Only the first component matters
	return color / num;	
}