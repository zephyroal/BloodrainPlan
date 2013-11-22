#ifdef LightModel_Debug_Model
#define LightModel_Map_Width 64
#define LightModel_Map_Height 128
#define LightModel_Map_Depth 8
float4 make_stripe_tex(float3 Pos : POSITION,float ps : PSIZE) : COLOR
{
	float v = 0;
	float nx = Pos.x+ps; // last column = 1 for all MIPs
	v = nx > Pos.y;
	return float4(v.xxxx);
}
texture g_MaterialTexture <
    string UIWidget =  "None";
    string ResourceType = "3D";
	float3 Dimensions = {LightModel_Map_Width,LightModel_Map_Height,LightModel_Map_Depth};
	string Function = "make_stripe_tex";
>;
#else
//--------------------------------------
//Material Information Map 
texture g_MaterialTexture <
    string ResourceName = "C:\\Program Files\\Autodesk\\3ds Max 9\\Scripts\\Startup\\Plugin\\FX\\Texture\\LightModel\\light_model_a8r8g8b8.dds";
    string UIName =  "Material Texture";
    string ResourceType = "3D";
>;

#endif
sampler3D g_MaterialMapSampler = sampler_state {
    Texture = <g_MaterialTexture>;

    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
}; 


void calcLightParams
		(
		float3 N,float3 V,float3 L,
		out float d,out float s
		)
{
	d=1.0f;
	s=0.0f;
 	if(g_NoLighting==false)
	{
		//light model calc
		float dot_NL=dot(N, L);
		float3 H = normalize(L + V);
		float dot_NH=dot(N,H);
		float4 l_material=tex3D(g_MaterialMapSampler,float3(dot_NL,dot_NH,g_Smoothness));

		d=l_material.x;
		s=l_material.a;
	}
}

void calcLight
	(
		float d,float s,
		out float3 diffuse_color,out float3 specular_color
	)
{
	float3 light_color=light_color_0.rgb;
	{
		float3 v1=light_color*d*0.7;
		float3 v2=g_AmbientColor.rgb+g_EmissiveColor.rgb;
		diffuse_color = v1+v2;
	}
	{
		specular_color = light_color.rgb*0.7*s*g_SpecularColor;
	}
}

