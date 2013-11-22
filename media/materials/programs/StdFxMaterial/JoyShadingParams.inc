#ifndef _JoyShadingParams_INC_
#define _JoyShadingParams_INC_

#ifdef _3DSMAX_
	//#include "StdFxMaterial/ParamNamedAuto.fxh"
	//#include "StdFxMaterial/ParamNamed.fxh"
#else
	#include "ParamNamedAuto.inc"
	#include "ParamNamed.inc"
#endif

/// light params
struct LightParams
{
		int			nType;
		int			nIndex;
		float3 	cDiffuse;
		float3 	cSpecular;
		float3 	cFilter;
		float3	vLight;
		
		float		fNdotL;
		float 	fNdotV;
		float		fFallOff;
		float		fOcclShadow;
		
		// final contribution
		float3	cOut;
		
		// ...
};

struct ShadingParams
{
		bool bNoLighting;
		bool bIsPlants;
		bool bDiffuse2Enable;
		bool bDiffuse2BlendEnable;
		int  nDiffuse2BlendType;
		bool bUseSkin;
		bool bBeastEnable;
		bool bNormalMapEnable;
		bool bUseVertexColor;
		bool bGrassEnable;
		bool bAlphaTestEnable;
		bool bZWriteEnable;
		bool bSpecularMapEnable;
		bool bHDREnable;
		int	 nShadingQuality;
		bool bWhiteScene;
};

/// vertex input
struct VertInput
{
		/// position
		float4 position;
		
		/// blend weights
		float4 blendIdx;
		float4 weight;
		
		/// normals
		float4 normal;
		float4 tangent;
		float4 vertexColor;
		
		/// texcoords
		float4 tc1;
		float4 tc2;
		float4 tc3;
		float4 tc4;
		float4 tc5;
		float4 tc6;
		float4 tc7;
};

/// vertex params
struct VertParams
{
		/// Inputs
		VertInput VI;
		
	  /// position
		float4 po;
		float4 pw;
		float4 pv;
		float4 ps;
		
		/// texcoords
		float4 baseTC;
		float4 externTC;
		float4 projTC;
		
		/// normals
		float4 vTangent;
		float4 vBinormal;
		float4 vNormal;
		float4 vView;
		
		/// colors
		float4 color;
		float4 sunRefl;
		float4 ambient;
		
		/// fog
		float4 fog;
};

/// fragment inputs
struct FragInput
{
		// texcoords
		float4 baseTC;
		float4 extTC1;
		float4 extTC2;
		float4 extTC3;
		
		// position
		float4 position;
		
		// nlv
		float3 vTangent;
		float3 vBinormal;
		float4 vNormal;
		float4 vView;
		
		float4 screenProj;
		float4 projTC;
		
		float4 color;
		float4 sunRefl;
		float4 ambient;
		
		float4 fog;
};

/// fragment options
struct FragOpts
{
		bool bUseVertexColor;
		int	 nShadingQuality;
		bool bHDREnable;
		bool bWhiteScene;
};

/// fragment params
struct FragParams
{
		FragInput 	FI;
		FragOpts 		Opt;
		LightParams LP;
		
		float4 cNormalRT;
		float4 cDiffuseRT;
		float4 cPositionRT;
		float4 cAddionalRT;
		
		float fReflectionAmount;
		float fReflectionBias;
		float fReflectionScale;
		float fFresnel;
		
		float fGammaVal;
		float fHDRPower;
		
		float fSmoothness;
		float fGlossiness;
		float fShiness;
		
		float fy;
		
		float4 cFinal;
		
		float4 cComplexity;
		
		/// custom
};

/// Common Shading Option Here

#endif