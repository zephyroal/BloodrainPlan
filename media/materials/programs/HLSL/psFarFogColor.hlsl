
struct OUTPUT
{
	float4 color : COLOR; 
};

OUTPUT main
(
	 float2    oUV    	      : TEXCOORD0
	,uniform   sampler2D Tex0 : register(s0)     //SceneDepth
	,uniform   sampler2D Tex1 : register(s1)     //VolumeCWDepth
	,uniform   float     camFarClipDistance
	,uniform   float4    FogColor
	,uniform   float4    FogDensity
)
{
   OUTPUT output;
	 ////////////////////////////////////////////////
 
   float sceneDepth    = (tex2D(Tex0, oUV)).w;
   float volumeCWDepth = (tex2D(Tex1, oUV)).x;
   
   if(sceneDepth < 0.1)
   	   sceneDepth  = camFarClipDistance;
   	   
	if(volumeCWDepth < 0.1f)
		volumeCWDepth = camFarClipDistance;
		
   float  finalDepth;
   
	 if(sceneDepth > volumeCWDepth)
	 		 finalDepth = volumeCWDepth;
	 else
	     finalDepth = sceneDepth;
	     	
   float factor  = 1;
    
   ///////////////////////////////////////////////
   factor = exp((-finalDepth * FogDensity.x * FogDensity.y)/ camFarClipDistance);
   
	 output.color.xyz =FogColor.xyz;
	
	 output.color.w = 1-factor;

	 return output;
}