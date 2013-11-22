#ifndef _JoyDiffuse_INC_
#define _JoyDiffuse_INC_

void Calc_Diffuse_Tex( inout FragParams params )
{
		// qtx todo : mipmap level 
#if g_SpaceDistortionEnable != 1
  #if g_FogEnable == 1
		params.cDiffuseRT = tex2D( DiffuseSampler, params.FI.baseTC.xy );
	#else
		float4 DifTex = float4( params.FI.baseTC.xy, 0, -params.FI.position.w/600 );
	  params.cDiffuseRT = tex2Dlod( DiffuseSampler, DifTex);
	#endif
	params.cDiffuseRT *= g_ColourFading;
#endif	
	
#if g_SpaceDistortionEnable == 1  
		// note : try do not sample textures together  
   	params.cDiffuseRT = tex2D( SceneSampler, params.FI.extTC2.xy );
   	params.cDiffuseRT.w = params.FI.extTC2.z;
#endif
		
		params.cDiffuseRT = Gamma( params.cDiffuseRT, params );
	    params.cDiffuseRT.rgb *= g_StageStyle;
#ifdef _3DSMAX_
	params.cDiffuseRT = g_Diffuse1Enable ? params.cDiffuseRT : params.FI.color;
#endif

#if g_GrassEnable != 0
	params.cDiffuseRT.rgb *= params.FI.vNormal.a;
#endif

	params.cFinal = params.cDiffuseRT;
}

#ifndef _3DSMAX_

void Calc_Terrain_BottomLayer( inout FragParams params )
{
	params.cDiffuseRT = tex2D( BottomLayerSampler, params.FI.baseTC.xy );
	params.cDiffuseRT = Gamma( params.cDiffuseRT, params );

	//只观察底层
#if g_MiddleLayerEnable != 1 && g_UpperLayerEnable != 1
	params.cFinal = params.cDiffuseRT;
#endif
}

void Calc_Terrain_MiddleLayer( inout FragParams params )
{
#if g_MiddleLayerEnable == 1
	float4 MiddleColor = tex2D( MiddleLayerSampler, params.FI.extTC1.xy );
	MiddleColor.rgb = Gamma( MiddleColor, params ).rgb;
	float MiddleFactor = tex2D( BlendLayerSampler, params.FI.extTC2.xy ).x;
	
	//只观察中层
#if g_BottomLayerEnable != 1 && g_UpperLayerEnable != 1
		params.cFinal = MiddleColor;
#endif
	
#if g_BottomLayerEnable == 1

		#if g_OldVersionEnable == 1
			params.cAddionalRT = lerp( params.cDiffuseRT, MiddleColor, MiddleColor.a );
		#else
			params.cAddionalRT = lerp( params.cDiffuseRT, MiddleColor, MiddleFactor );
		#endif
		
		//只有底层和中层
		#if g_UpperLayerEnable != 1
			params.cFinal = params.cAddionalRT;
		#endif
	#endif
	
#endif
}

void Calc_Terrain_UpperLayer( inout FragParams params )
{
#if g_UpperLayerEnable == 1 
	float	 UpperFactor 	= tex2D( BlendLayerSampler, params.FI.extTC2.xy ).y;
	params.fy = UpperFactor;
	float4 UpperColor 	= tex2D( UpperLayerSampler, params.FI.extTC1.zw );
	UpperColor.rgb = Gamma( UpperColor, params ).rgb;
	
	//只观察上层
	#if g_MiddleLayerEnable != 1 && g_BottomLayerEnable != 1
		params.cFinal = UpperColor;
	#endif
	
	//三层都有
	#if g_BottomLayerEnable == 1 && g_MiddleLayerEnable == 1
		params.cFinal = lerp( params.cAddionalRT, UpperColor, UpperFactor );
	#endif
	
	//只有底层和上层
	#if g_BottomLayerEnable == 1 && g_MiddleLayerEnable != 1
		params.cFinal = lerp( params.cDiffuseRT, UpperColor, UpperFactor );
	#endif
	
#endif
}

#endif

#endif