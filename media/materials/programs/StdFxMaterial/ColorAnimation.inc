bool g_ColorAnimationEnable <
	string UIName = "Color Animation Enable";
> = false;


float g_ColorAnimationSpeed <
	string UIName = "Color Animation Speed";
	float UIMin = 0.001f;
	float UIMax = 1000.0f;
	float UIStep = 0.01f;
> = 0.2f; //loop per 1/0.2=5 second



//color animation
texture g_ColorAnimationTexture < 
	string UIName = "Color Animation Map ";
	int Texcoord = 0;
	int MapChannel = 1;    
>;

sampler2D g_ColorAnimationSampler = sampler_state
{
	Texture = <g_ColorAnimationTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
  	AddressU = WRAP;
  	AddressV = WRAP;
};

float4 getColorAnimationColor()
{
	float4 color_animation=float4(1,1,1,1);
	if(g_ColorAnimationEnable)
	{
		//float2 color_uv=float2(g_ColorAnimationSpeed * time,0.5f);
		float color_u=time*g_ColorAnimationSpeed;
		color_animation=tex2D(g_ColorAnimationSampler,float2(color_u,0.0));
	}
	return color_animation;
}