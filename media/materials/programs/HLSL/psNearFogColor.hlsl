struct OUTPUT
{
	float4 color : COLOR; 
};

OUTPUT main
(
	 float4    objPos    : TEXCOORD1
	,float4    UV   		 : TEXCOORD2
	,uniform   sampler2D Tex0      : register(s0)      //SceneDepth
	,uniform   sampler2D Tex1      : register(s1)      //VolumeCWDepth
	,uniform   sampler2D Tex2			 : register(s2)			 //CloudNoise
	,uniform   float4x4  wvpMat
	,uniform   float4    viewportSize
	,uniform   float     camFarClipDistance
	,uniform   float4    fogColor
	,uniform   float4    fogParam
	,uniform   float4    UVScale
)
{
   OUTPUT output;
	 ////////////////////////////////////////////////
	 
	 float4 projPos = mul(wvpMat, objPos);
	 float  depth   = projPos.w;
   float  u       = (projPos.x/projPos.w * 0.5) + 0.5 + 0.5 * viewportSize.z;
   float  v       = -(projPos.y/projPos.w * 0.5) + 0.5 +  0.5 * viewportSize.w;
   float2 tex     = float2(u,v);
   
   ///////////////////////////////////////////////
 
   float sceneDepth    = (tex2D(Tex0, tex)).w;
   float volumeCWDepth = (tex2D(Tex1, tex)).x;
   
   if(sceneDepth < 0.1)
   	   sceneDepth  = camFarClipDistance;
   
   if(volumeCWDepth < 0.1f)
		volumeCWDepth = camFarClipDistance;
		
   float  finalDepth;
   
	 if(sceneDepth > volumeCWDepth)
	 		 finalDepth = volumeCWDepth;
	 else
	     finalDepth = sceneDepth;
	     	
   float4 factor  = 1;
   float  delta   = finalDepth - (projPos.w);
   //////////////////////////////////////////////////////////////////
   
   float2 direction = fogParam.zw;
   float  waveSpeed = fogParam.y;
   float  timePast  = UV.z;
   float2 vTranslation = timePast * direction * waveSpeed;
   float4 vTex         = float4( UV.xy, 0, 1 );
   float4 wave0;
   float4 wave1;
   
	 wave0.xy = vTex.xy*2*1.0 + vTranslation*2.0;
   wave0.wz = vTex.xy*1*1.0 + vTranslation*3.0;
   wave1.xy = vTex.xy*2*2.0 + vTranslation*2.0;
   wave1.wz = vTex.xy*2*4.0 + vTranslation*3.0;
   //////////////////////////////////////////////////////////////////
   
   float4 bumpColor;
   
   bumpColor.x = tex2D(Tex2, wave0.xy * UVScale.xy).x;           
   bumpColor.y = tex2D(Tex2, wave0.wz * UVScale.xy).x;           
   

   bumpColor.z = tex2D(Tex2, wave1.xy * UVScale.xy).x;
   bumpColor.w = tex2D(Tex2, wave1.wz * UVScale.xy).x;
    
	 float noise = (bumpColor.x + bumpColor.y + bumpColor.z + bumpColor.w)/ 4.0;
   //////////////////////////////////////////////////////////////////
   
   float affectNoise = (noise - 0.2) * 10;	
   
   if(delta > -0.00001) 
   {
   		 factor = exp((-delta * fogParam.x) / camFarClipDistance);
   		 factor = factor + affectNoise;
   		 //factor = factor * (1 + affectNoise);
   }	 
  
   //////////////////////////////////////////////////////////////////
   
	 output.color.xyz = fogColor.xyz;
	 
	 output.color.w = saturate(1 - factor);
	 return output;
}