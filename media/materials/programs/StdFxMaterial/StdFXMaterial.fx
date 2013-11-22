string ParamID = "0x003"; 

#define _3DSMAX9_

float Script : STANDARDSGLOBAL <
    string UIWidget = "none";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color"; 
    string Script = "Technique=Main;";
> = 0.8; 
     
/// max包含的特殊头文件 
#include "Max_FX_Common.inc"
#include "ParamNamedAuto.fxh" 
#include "ParamNamed.fxh" 
#include "Utility.inc" 
#include "LightModel.inc"
#include "ColorAnimation.inc"
 
/// Common
#include "../JoyCommon.inc"
#include "../JoyShadingParams.inc" 

#ifdef _3DSMAX9_
	#include "../JoySkin.inc"
	#include "../JoyCalcPos.inc"
	#include "../JoyCalcNormal.inc"
	#include "../JoyCalcUV.inc"
	#include "../JoyCalcView.inc"
	#include "../JoyDiffuse.inc"
	#include "../JoyDiffuse2Blend.inc"
	#include "../JoyTexKill.inc"
	#include "../JoyNormalMap.inc"
	#define __PS__
	#include "../JoyLight.inc"  
	#include "../JoyParallax.inc"
	#include "../JoyEnvMap.inc"
#else
	#include "JoySkin.inc"
	#include "JoyCalcPos.inc"
	#include "JoyCalcNormal.inc"
	#include "JoyCalcUV.inc"
	#include "JoyCalcView.inc" 
	#include "JoyDiffuse.inc"
	#include "JoyDiffuse2Blend.inc"
	#include "JoyTexKill.inc"
	#include "JoyNormalMap.inc"
	#define __PS__
	#include "JoyLight.inc" 
	#include "JoyParallax.inc"
	#include "JoyEnvMap.inc"
#endif

struct Std_Model_In
{
	float4 P		: POSITION;
	float3 N		: NORMAL;
	float3 T		: TANGENT;    
	float3 B		: BINORMAL;
	float2 UV1		: TEXCOORD0;
	float2 UV2		: TEXCOORD1;
	float2 UV3		: TEXCOORD2;
	float4 Color	: TEXCOORD3;
	float3 Alpha	: TEXCOORD4;
};

struct Std_Model_Out
{
	float4 hP			: POSITION;
	float4 Color		: Color;
	float4 UV1			: TEXCOORD0;
	float4 UV2			: TEXCOORD1;
	float3 N			: TEXCOORD2;
	float3 T			: TEXCOORD3;
	float3 B			: TEXCOORD4;
	float3 L			: TEXCOORD5;
	float3 V			: TEXCOORD6;
	float4 P			: TEXCOORD7;
};

struct Std_Ps_Out
{
	float4 Color		: COLOR;  
};

VertInput fmt_input( Std_Model_In IN )
{
	VertInput VI 	= (VertInput)0;
	VI.position 	= float4( IN.P.xyz, 1 );
	VI.normal			= float4( IN.N.rgb, 0 );
	VI.tangent		= float4( IN.T, 0 ); 
	VI.tc1				= IN.UV1.xyxy;
#if g_UseVertexColor
	VI.vertexColor= IN.Color;
#endif

	return VI;
} 
  
Std_Model_Out VS( Std_Model_In IN ) 
{ 
	Std_Model_Out OUT = (Std_Model_Out)0;
	VertParams params	= (VertParams)0;  
	params.VI = fmt_input( IN );
	
	/// calc position
	Calc_Position( params );  
	
	/// calc normal
	Calc_Normal( params );
	 
	/// calc uv
	Calc_Uv( params );    
	
	/// calc view 
	Calc_View( params );
	 
  OUT.hP= params.ps;
	OUT.P = params.pw;
	   
	float3 L = (light_position_0.xyz - params.pw.xyz);
	
	OUT.L = normalize(L);
	OUT.V = params.vView;

	if(g_NormalEnable) 
	{
		//trans normal to world space
		OUT.N= params.vNormal.xyz;
		OUT.T= params.vTangent.xyz;
		OUT.B= params.vBinormal.xyz;
	}
	else
	{
		//trans normal to world space
		OUT.N= params.vNormal.xyz; 
	}
  OUT.UV1.xy = params.baseTC.xy;
  
  if ( g_UseVertexColor )
	{
		OUT.Color = IN.Color;
	}
	else
	{ 
		OUT.Color = float4(1,1,1,1);
	}
  return OUT;
}
 
FragInput fmt_input( Std_Model_Out IN ) 
{
	FragInput FI = (FragInput)0;
	FI.baseTC			= IN.UV1;  
	FI.vTangent		= IN.T.xyz; 
	FI.vBinormal	= IN.B.xyz; 
	FI.vNormal		= float4( IN.N.xyz, 1 ); 
	FI.vView			= float4( normalize( IN.V.xyz ), 1 );
	FI.color			= IN.Color;
	FI.position		= IN.P;
	return FI; 
}

Std_Ps_Out fmt_output( FragParams params ) 
{
	Std_Ps_Out OUT = (Std_Ps_Out)0;
	OUT.Color = float4( params.cFinal.rgb, params.cDiffuseRT.a * params.FI.color.a ); 
	return OUT;
} 
 
Std_Ps_Out PS( Std_Model_Out IN )  
{
	Std_Ps_Out OUT=(Std_Ps_Out)0;
	
	FragParams params = (FragParams)0;  
	params.FI = fmt_input( IN );
	
	/// 设置渲染参数
	fmt_rop( params );
	
	///  parallax
	Clac_Parallax( params );
	
	/// diffuse rt 
	Calc_Diffuse_Tex( params ); 
	
	/// diffise 2
	Calc_Diffuse2_Blend( params );
	 
	/// alpha rejection
	Tex_Kill( params ); 
	 
	/// bump normal
	Bump_Normal( params );
	
	/// env map
	Calc_EnvMap( params );
	
	/// lighting
	Calc_Light_In_Ps_FL( params );
	
	return fmt_output( params ); 
}

technique Main <
	string Script = "Pass=p0;"; 
>   
{ 
    pass p0 <string Script = "Draw=geometry;";>   
    {
        VertexShader	= compile vs_3_0 VS(); 
        PixelShader		= compile ps_3_0 PS(); 
         
        AlphaBlendEnable = <g_AlphaBlendEnable>;
        
        //SrcBlend = (g_BlendOP==1?2:(g_BlendOP==2?9:(g_BlendOP==3?3:5)));
        //DestBlend = (g_BlendOP==1?2:(g_BlendOP==2?1:(g_BlendOP==3?4:6)));
        
        SrcBlend = <g_SrcBlend>; 
        DestBlend = <g_DestBlend>;
        
				BlendOP = ADD;
        
        AlphaTestEnable = <g_AlphaTestEnable>;
        AlphaRef = <g_AlphaRef>;
        AlphaFunc = <g_AlphaFunc>;
        
        ColorWriteEnable=(g_ColorWriteEnable?0xf:0);
        
        ZEnable = <g_ZEnable>;
        ZFunc = <g_ZFunc>;
        ZWriteEnable = <g_ZWriteEnable>;
        
				CullMode = <g_CullMode>;
				
				
				DepthBias = (g_DepthBiasEnable?-0.00001:0.0);
				SlopeScaleDepthBias = (g_DepthBiasEnable?-0.00001:0.0);
    }
}
