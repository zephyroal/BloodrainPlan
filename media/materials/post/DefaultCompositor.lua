execute( 'common.def' );
execute( 'scene.def' );
execute( 'volumeFog.def' );
execute( 'dof.def' );
execute( 'hdr.def' );
execute( 'water.def' );
execute( 'hfog.def' );
execute( 'hsv.def' );
execute( 'dbg.def' );
execute( 'shadow.def' );
execute( 'godray.def');
execute( 'aa.def' );
execute( 'motion_blur.def' );
execute( 'distortion.def' );
execute( 'scenebrightAjust.def');
execute( 'mask.def');
execute( 'Heat_Haze.def' );
execute( 'fadedeath.def')

local Compositor	=	_G.Compositor.Compositor;
local Technique		=	_G.Compositor.Technique;
local Texture			=	_G.Compositor.Texture;
local Target			=	_G.Compositor.Target;
local Clear				=	_G.Compositor.Clear;
local Stencil			=	_G.Compositor.Stencil;
local RenderScene	=	_G.Compositor.RenderScene;
local RenderQuad	=	_G.Compositor.RenderQuad;

local main_buffer = "rt_buffer";

-- 注意：当HDR开启时所有buffer格式会自动切换到PF_FLOAT16_RGBA

MainCompositor = {};
MainCompositor.createCompositor=
Compositor
{
	name = "Main";
	Technique
	{
		scheme	=	"";
		name		=	"defaultTech";
				
		-- scene.def
		Texture( SceneTexDef );
		Texture( Swap1TexDef );
		Texture( Swap2TexDef );
		
		Target( RT_ClearRed );
		
		Target( RT_ZWrite );
		Target( RT_Terrain );
		Target( RT_SkyDome );	
		Target( RT_Sky );
		Target( RT_AlphaTest );
		Target( RT_TmpTransparency );
				
		-- receiveshadow
		Target( RT_ShadowMap_Receive );
		--Target( RT_ShadowMap_Blend );
		Target( RT_ShadowMap_CopySwapToMain );
		
		-- logicModel
		Target( RT_Actor );
		
		-- water.def
		Target( RT_CopyMainToSwap_Clear);
		Target( RT_CopyMainToSwap_Water );
		Target( RT_WaterFog );
		Target( RT_Water );
		
		-- distortion.def  特效空间扭曲
		Target( RT_CopyMainToSwap_Distortion );
		Target( RT_Effect_Distortion );
		
		
		Target( RT_UnSorted );
		
		
		-- volumeFog.def
		Texture( VolumeFogTexDef );
		Target(RT_VolumeCCW_Clear);
		Target( RT_VolumeCCW );
		Target(RT_VolumeFog_Clear);
		Target( RT_VolumeFog );
		Target(RT_VolumeFogInner_Clear);
		Target( RT_VolumeFogInner );
		Target( RT_CopyMainToSwap2_VolumeFog );
		Target( RT_VolumeFogCombine );
		
		-- hdr.def
		Texture( Hdr64TexDef );
		Texture( Hdr16TexDef );
		Texture( Hdr4TexDef );
		Texture( Hdr1TexDef );
		Texture( HdrPre1TexDef );
		Texture( HdrAdapterTexDef );
		Texture( HdrBrightTexDef );
		Texture( HdrBloomHTexDef );
		Texture( HdrBloom1TexDef );
		Texture( HdrBloom1HTexDef );
		Texture( HdrBloom2TexDef );
		Texture( HdrBloom2HTexDef );
		Texture( HdrComposeTexDef );
		
		-- hdr.def
		Target( RT_HDR_DownSacleScene ); -- 对原场景进行模糊处理
		Target( RT_HDR_LumSample );
		Target( RT_HDR_DownScale16 );
		Target( RT_HDR_DownScale4 );
		Target( RT_HDR_DownScale1 );
		Target( RT_HDR_BlurPreInit );
		Target( RT_HDR_Adapter ); -- 计算曝光
		Target( RT_HDR_Copy );
		Target( RT_HDR_Bright );
		Target( RT_HDR_BloomH );
		Target( RT_HDR_BloomV );
		Target( RT_HDR_BloomScale );
		Target( RT_HDR_BloomH2 );
		Target( RT_HDR_BloomV2 );
		Target( RT_HDR_ReScale1 );
		Target( RT_HDR_ReScale2 );
		Target( RT_HDR_ReScale3 );
		Target( RT_HDR_Compose );
		Target( RT_HDR_Final );
		Target( RT_HDR_CopySwapToMain );
		
		-- hfog.def
		Target( RT_HFogCombine );
		Target( RT_CopySwapToMain_HFog );
		
		-- scene.def 渲染透明对象
		Target( RT_AlphaBlend );
		
		-- godray.def
		--[[
		Texture( GodRayMaskTexDef );
		Target( RT_GodRayMask );
		Target( RT_GodRay );
		Target( RT_CopySwapToMain_GR );
		--]]
		
		Texture( MonBlurTexDef );
		Target( RT_MonblurInitial );
		Target( RT_MonBlurCombine );
		Target( RT_MonBlurCopy );
		Target( RT_MonBlurCopyToMain );
		
		-- dof.def
		Texture( BlurTexDef );
		Target( RT_DofBlur );
		Target( RT_Dof );
		Target( RT_CopySwapToMain_DOF );
		
		-- hsv.def
		Target( RT_HSV );
		Target( RT_CopySwapToMain_HSV );
		
		-- HeatHaze
		Target( RT_HeatHaze );
		Target( RT_HeatHazeCopyToMain );
		
		-- aa.def
		Target( RT_FXAA );
		Target( RT_CopySwapToMain_AA );
		
		--SceneBrightnessAjust
		Target( RT_SceneBrightAjust );
		Target( RT_CopySwapToMain_ScnBriAdjust );			
		Target( RT_ReRenderObj );							
		
		--fade
		Target( RT_FdeathCombine);
		Target( RT_CopySwapToMain_FDeath);
		-- dbg.def
		--Target( RT_Dbg_CopyMain1 );
		
		-- common.def
		Target( RT_FinalOutput );
	};
}

-- 直接在这里创建后处理
local main_compositor={};
MainCompositor.createCompositor(main_compositor);
local cm=Ogre.CompositorManager.getSingletonPtr();
local compositor_name="Main";
local c;
c=cm:getByName(compositor_name,"General");
c:unload();
c:load(false);

-- 更改渲染模式
g_Render_Mode = "default"; 