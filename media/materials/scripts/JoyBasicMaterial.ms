--[[
-----------------------------------------------------------------------------
This Source File Is Part Of [Cross Platform Utility Library]
 
Copyright (c) winslylord@qtx All Rights Reserved
 
Mail				: winslylord@me.com
 
Description			: Lua Exports
 
Target Platform		: All
 
Ver					: 1.0.0
-----------------------------------------------------------------------------
--]]

local Material 		= OgreMaterial.Material;
local Technique		= OgreMaterial.Technique;
local Pass				= OgreMaterial.Pass;
local Program			= OgreMaterial.Program;
local Program_Ref	= OgreMaterial.Program_Ref;
local TextureUnit	= OgreMaterial.TextureUnit;

-- Cache Group todo qtx
local CacheGroup	= OgreMaterial.CacheGroup;
local vs_cache_group= "basic_material_vs";
local ps_cache_group= "basic_material_ps";

-- 不同Technique先直接复制，copy_from语义可能有歧义

local TextureUnit_Diffuse1 =
{
	copy_from				=	BaseTextureUnit;
	name						=	"diffuse1";
	con							=	true;
	sampler_register_name	=	"SD";
	texture 					= getUnAnimationTexture	("ForceEnable","T_D");
	tex_address_mode	=	getTextureAddressMode("T_D_Address_Mode","T_D_Address_Mode");
	-- filtering 				= getNormalFilter();
	scroll_anim				= getScrollAnim( "T_D_ScrollAnim_Enable", "T_D_Anim_X", "T_D_Anim_Y", "T_D_FixedTransform" );
	rotate_anim				= getRotateAnim( "T_D_RotateAnim_Enable", "T_D_Anim_Rotate", "T_D_FixedTransform" );
	scale_anim				= getScaleAnim( "T_D_ScaleAnim_Enable", "T_D_Scale_X", "T_D_Scale_Y", "T_D_FixedTransform" );
	type							=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_Diffuse2 =
{
	copy_from				=	BaseTextureUnit;
	name						=	"diffuse2";
	con							=	context_bool("g_Diffuse2Enable",1);
	sampler_register_name	=	"SD2";
	texture 					= 	getUnAnimationTexture	("ForceEnable","T_D2");
	tex_address_mode	=	getTextureAddressMode("T_D2_Address_Mode","T_D2_Address_Mode");
	-- filtering 				= 	getDiffuseFilter();
	scroll_anim				= getScrollAnim( "T_D2_ScrollAnim_Enable", "T_D2_Anim_X", "T_D2_Anim_Y", "T_D2_FixedTransform" );
	rotate_anim				= getRotateAnim( "T_D2_RotateAnim_Enable", "T_D2_Anim_Rotate", "T_D2_FixedTransform" );
	scale_anim				= getScaleAnim( "T_D2_ScaleAnim_Enable", "T_D2_Scale_X", "T_D2_Scale_Y", "T_D2_FixedTransform" );
	type					=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_DiffuseBlend =
{
	copy_from				=	BaseTextureUnit;
	name						=	"diffuse2";
	con							=	context_bool("g_Diffuse2BlendEnable",1);
	sampler_register_name	=	"SDM";
	texture 					= 	getUnAnimationTexture	("ForceEnable","T_DM");
	tex_address_mode		=	getTextureAddressMode("g_Diffuse1_AddressU","g_Diffuse1_AddressV");
	-- filtering 				= 	getDiffuseFilter();
	type					=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_NormalMap =
{
	copy_from				=	BaseTextureUnit;
	name						=	"normal";
	con							=	context_bool("g_NormalMapEnable",1);
	sampler_register_name	=	"SNM";
	texture 					= 	getUnAnimationTexture	("ForceEnable","T_DN");
	tex_address_mode		=	getTextureAddressMode("g_Diffuse1_AddressU","g_Diffuse1_AddressV");
	-- filtering 				= 	getDiffuseFilter();
	type					=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_SpecularMap =
{
	copy_from				=	BaseTextureUnit;
	name						=	"specular";
	con							=	context_bool("g_SpecularMapEnable",1);
	sampler_register_name	=	"SSP";
	texture 					= 	getUnAnimationTexture	("ForceEnable","T_DS");
	tex_address_mode		=	getTextureAddressMode("g_Diffuse1_AddressU","g_Diffuse1_AddressV");
	-- filtering 				= 	getDiffuseFilter();
	type					=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_BeastMap = 
{
  copy_from             = BaseTextureUnit;
  name									=	"beastmap";
	con										=	context_bool("g_BeastMapEnable",1);
	sampler_register_name	=	"SBM";
	texture 							= function() return material_define["T_BMT"] end;
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_Complexity= 
{
  copy_from             = BaseTextureUnit;
  name									=	"complexity";
	con										=	function() return g_IsReleaseMode == 0 end;
	sampler_register_name	=	"SCC";
	texture 							= function() return "complexity.dds" end;
	--filtering 						= getPointFilter();
	type									=	Ogre.Global.TEX_TYPE_1D;
};

mdefs = {};

get_program_name_vs = function( shading_mode )

return function()

		_G.Utility.CopyTable( material_define, mdefs );
		local names = {};
		
		-- 提前设定不同模式下的宏切换
		if ( shading_mode ~= "default" ) then
	  	mdefs.g_NoLighting = 1;
	  else
	  	mdefs.g_BeastMapEnable = 0;
	  end;
	  
	  if ( Hardware_SM3 == 1 ) then
	  	mdefs.g_SM3 = 1;
	  	names.g_SM3 = "SM3";
	  end;
	  
	  mdefs.g_IsReleaseMode = g_IsReleaseMode or 0;
	  names.g_IsReleaseMode = "RM";
	  
	  if ( mdefs.g_IsPlants == 1 ) then
	  	names.g_IsPlants = "PLT";
	  end;
	  
	  if ( mdefs.g_Diffuse2Enable == 1 ) then
		  names.g_Diffuse2Enable = "D2E";
		end;
		
		if ( mdefs.g_Diffuse2BlendEnable == 1 ) then
		  names.g_Diffuse2BlendEnable = "D2BE";
		  names.g_Diffuse2BlendType = "D2BT";
		end;
		
		if ( mdefs.g_UseSkin == 1 ) then
			if (Hardware_SM3~=1) then
				mdefs.g_UseSkin = 0;
			else
				names.g_UseSkin = "USN";
			end;
		end;
		
		if ( mdefs.g_BeastMapEnable == 1 ) then
		  names.g_BeastMapEnable = "BMT";
		end;
		
		if ( mdefs.g_NormalMapEnable == 1 ) then
			names.g_NormalMapEnable = "NME";
		end;
		
		if ( mdefs.g_UseVertexColor == 1 ) then
			names.g_UseVertexColor = "UVC";
		end;
		
		if ( mdefs.g_GrassEnable ~= 0 ) then
			names.g_GrassEnable = "GE";
		end;
		
		if ( mdefs.T_D_ScrollAnim_Enable == 1 or mdefs.T_D_RotateAnim_Enable == 1 or mdefs.T_D_ScaleAnim_Enable == 1 ) then
			mdefs.g_Texture1EffectEnable = 1;
			material_define.g_Texture1EffectEnable = 1;
			names.g_Texture1EffectEnable = "T1EE";
		end;
		
		if ( mdefs.T_D2_ScrollAnim_Enable == 1 or mdefs.T_D2_RotateAnim_Enable == 1 or mdefs.T_D2_ScaleAnim_Enable == 1 ) then
			mdefs.g_Texture2EffectEnable = 1;
			material_define.g_Texture2EffectEnable = 1;
			names.g_Texture2EffectEnable = "T2EE";
		end;
		
		local temp1	=	{ "JBMVS_" .. tostring(shading_mode); };
		local temp2	=	{"__NO_DEBUG" };
		for k,v in pairs(names) do
			temp1[#temp1+1]=";"..v.."="..tostring((mdefs[k] or 0));
			temp2[#temp2+1]=";"..k.."="..tostring((mdefs[k] or 0));
		end;
		
		local v1 = table.concat(temp1);
		local v2 = table.concat(temp2);
		
		context.vertex_program_name  	= v1;
		context.vertex_program_define	= v2;
		
		return v1;
	end;
end;

get_program_name_ps = function( shading_mode )

return function()

		_G.Utility.CopyTable( material_define, mdefs );

		local names = {};
		
		-- 提前设定不同模式下的宏切换
		if ( shading_mode ~= "default" ) then
	  	mdefs.g_NoLighting = 1;
	  else
	  	mdefs.g_BeastMapEnable = 0;
	  end;
	  
	  if ( shading_mode == "trans" ) then
	  	mdefs.g_ForceTrans = 1;
	  	names.g_ForceTrans = 1;
	  end;
	  
	  if ( Hardware_SM3 == 1 ) then
	  	mdefs.g_SM3 = 1;
	  	names.g_SM3 = "SM3";
	  end;
	  
	  mdefs.g_IsReleaseMode = g_IsReleaseMode or 0;
	  names.g_IsReleaseMode = "RM";
	  
	  mdefs.g_CapsLevel = Hardware_CapsLevel;
	  names.g_CapsLevel = "CL";
		
		 -- 最大支持的光源数量
	  mdefs.MAX_LIGHT_NUM = Max_Light_Num or 8;
	  names.MAX_LIGHT_NUM = "MLT";
	  
	  if ( mdefs.g_IsPlants == 1 ) then
	  	names.g_IsPlants = "PLT";
	  end;
		
		if ( mdefs.g_NoLighting == 1 ) then
		   names.g_NoLighting = "NL";
		end;
		
		if ( mdefs.g_BeastMapEnable == 1 ) then
		  names.g_BeastMapEnable = "BMT";
		end;
		
		if ( mdefs.g_AlphaTestEnable == 1 ) then
		   names.g_AlphaTestEnable = "AT";
		end;
		
		if ( mdefs.g_ZWriteEnable == 1 and mdefs.g_AlphaBlendEnable == 0) then
			names.g_ZWriteEnable = "ZW";
		end;
		
		names.g_FogEnable = "FE";
		
		if ( mdefs.g_Diffuse2Enable == 1 ) then
		  names.g_Diffuse2Enable = "D2E";
		end;
		
		if ( mdefs.g_Diffuse2BlendEnable == 1 ) then
		  names.g_Diffuse2BlendEnable = "D2BE";
		  names.g_Diffuse2BlendType = "D2BT";
		end;
		
		if ( mdefs.g_NormalMapEnable == 1 ) then
			names.g_NormalMapEnable = "NME";
		end;
		
		if ( mdefs.g_SpecularMapEnable == 1 ) then
			names.g_SpecularMapEnable = "SME";
		end;
		
		if ( mdefs.g_GrassEnable ~= 0 ) then
		  names.g_GrassEnable = "GE";
		end;

		
		local temp1={ "JBMPS_" .. tostring(shading_mode); };
		local temp2={"__NO_DEBUG" };
		for k,v in pairs(names) do
			temp1[#temp1+1]=";"..v.."="..tostring((mdefs[k] or 0));
			temp2[#temp2+1]=";"..k.."="..tostring((mdefs[k] or 0));
		end;
		
		for k,v in ipairs(context.samplers) do
			temp2[#temp2+1]=";"..v.."=s"..tostring(k-1);
		end;

		local v1	= table.concat(temp1);
		local v2	= table.concat(temp2);
		
		context.pixel_program_name		=	v1;
		context.pixel_program_define	=	v2;

		return v1;
	end;
end;

local Program_VS_Default=
{
	cache_group 	= vs_cache_group;
	program_type	= Ogre.Global.GPT_VERTEX_PROGRAM;
	source				= "JoyBasicMaterial.vs";
	entry_point		= "main";
	language			= "hlsl";
	target				= function() 
										if ( Hardware_SM3 == 1 ) then
											return "vs_3_0";
										else
											return "vs_2_0";
										end;
									end;
	
	column_major_matrices	= false;
	preprocessor_defines	= function() return context.vertex_program_define; end;
	
	includes_skeletal_animation = function()
		if ( Hardware_SM3==1 ) then
			if (material_define.g_UseSkin==1) then
				return true;
			else
				return false;
			end;
		else
			return false;
		end;
	end;
	
	named_auto_constants=
	{
		{"world_matrix"};
		{"worldview_matrix"};
		{"inverse_transpose_world_matrix"};
		{"transpose_world_matrix"};
		{"worldviewproj_matrix"};
		{"camera_position"};
		{"light_position"};
		{"fog_colour"};
		{"fog_params"};
		{"world_matrix_array_3x4",Ogre.Global.ACT_WORLD_MATRIX_ARRAY_3x4, 68};
		{"view_matrix"};
		{"projection_matrix"};
		{"g_BoneNum",Ogre.Global.ACT_BONE_NUM,0};
		{"viewproj_matrix"};
		{"texture_matrix_0",Ogre.Global.ACT_TEXTURE_MATRIX, 0};
		{"texture_matrix_2",Ogre.Global.ACT_TEXTURE_MATRIX, 1};
	};
	
	named_auto_constants_real=
	{
		{"time"},
		{"time_0_1",Ogre.Global.ACT_TIME_0_1,1000},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants = function()
		local t = {};
		
		if ( material_define.g_UseSkin == 1 ) then
			t["g_BoneNum"] = Ogre.Vector4( 0 or 1,0,0,0 );
		end;
		
		if ( material_define.g_GrassEnable ~= 0 ) then
			t["grassOffset"] = Ogre.Vector4( 0,0,0,0 );
			t["grassParam"] = Ogre.Vector4( 0,0,0,0 );
		end;
		
		if(material_define.g_BeastMapEnable==1)then
			t["BeastMapBias"] = Ogre.Vector4(0, 0, 1, 1);
		end;
		
		t["use_instance_rendering"] = Ogre.Vector4(0,0,0,0);

		return t;
	end;
};

local Program_VS_Pre=
{
	copy_from			= Program_VS_Default;
	source				= "JoyBasicMaterial.vs";
};

local Program_VS_Deffered=
{
	copy_from			= Program_VS_Default;
	source			= "JoyBasicMaterial_DF.vs";
};

local Program_VS_SceneDepth=
{
	copy_from			= Program_VS_Default;
	source			= "JoySceneDepth.vs";
};

local Program_VS_TexDensity=
{
	copy_from			= Program_VS_Default;
	source			= "JoyTexDensity.vs";
};

local Program_VS_Trans=
{
	copy_from			= Program_VS_Default;
	source			= "JoyBasicMaterial.vs";
};

local Program_Ref_VS_Default=
{
	con = function()
		if ( context.vertex_program ) then
			return true;
		end;
		return false;
	end;
	
	program_name = function()
		return context.vertex_program_name;
	end;
	
	program_type = Ogre.Global.GPT_VERTEX_PROGRAM;
	
	shared_params_ref = function()
		local t={};
		--[[
		if(material_define.g_WindEnable==1)then
			table.insert(t,"WindParameters");
		end;
		--]]
		return t;
	end;
	
	named_constants = function()
		local t={};
		
		if ( material_define.g_UseSkin == 1 ) then
			t["g_BoneNum"] = Ogre.Vector4( 0 or 1,0,0,0 );
		end;
				
		if(material_define.g_BeastMapEnable==1)then		
			t["BeastMapBias"]  = Ogre.Vector4(material_define.g_BeastXBias, material_define.g_BeastZBias, material_define.g_BeastImgScale , 1 );
		end;
		
		t["use_instance_rendering"] = Ogre.Vector4(0,0,0,0);
		
		return t;
	end;
}; 

-- 基础ps模板
local Program_PS_Default=
{
	doc						= "pixel program";
	cache_group 	= ps_cache_group;
	program_type	= Ogre.Global.GPT_FRAGMENT_PROGRAM;
	source				= "JoyBasicMaterial.ps";
	entry_point		= "main";
	language			= "hlsl";
	target				= function() 
										if ( Hardware_SM3 == 1 ) then
											return "ps_3_0";
										else
											return "ps_2_0";
										end;
									end;
	
	column_major_matrices	=	true;
	preprocessor_defines	=	function() return context.pixel_program_define; end;
	
	named_auto_constants=
	{
		{"world_matrix"};
		{"inverse_transpose_world_matrix"};
		{"worldviewproj_matrix"};
		{"ambient_light_colour"};
		{"surface_diffuse_colour",Ogre.Global.ACT_SURFACE_EMISSIVE_COLOUR,0};
		{"derived_scene_colour"};
		{"light_diffuse_colour_power_scaled"};
		{"light_specular_colour_power_scaled"};
		{"light_position_array",Ogre.Global.ACT_LIGHT_POSITION_ARRAY,Max_Light_Num};
		{"light_diffuse_colour_power_scaled_array",Ogre.Global.ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,Max_Light_Num};
		{"light_specular_colour_power_scaled_array",Ogre.Global.ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY,Max_Light_Num};
		{"derived_light_diffuse_colour_array",Ogre.Global.ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY,Max_Light_Num};
		{"derived_light_specular_colour_array",Ogre.Global.ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY,Max_Light_Num};
		{"derived_scene_colour"};
		{"light_attenuation"};
		{"light_attenuation_array",Ogre.Global.ACT_LIGHT_ATTENUATION_ARRAY,Max_Light_Num};

		{"light_position"};
		{"surface_emissive_colour"};
		{"light_position_object_space"};
		{"light_position_view_space"};

		{"camera_position"};
		{"camera_position_object_space"};
		
		{"surface_specular_colour"};
		{"surface_shininess"};

		--{"surface_diffuse_colour_per_object",Ogre.Global.ACT_CUSTOM,0};
		--{"time_in_effect",Ogre.Global.ACT_CUSTOM,1};
	};
	
	named_auto_constants_real=
	{
		{"time"},
		{"light_count"},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants=function()
		local t={};
		
		if ( material_define.g_NoLighting == 0 ) then
			t["g_LightingParams"]=Ogre.Vector4(material_define.g_Smoothness or 1.0,material_define.g_Shininess or 1.0,100,0.0);
		end;
		
		if ( material_define.g_NormalMapEnable == 1 ) then
			t["normalBumpPower"] = material_define.g_NormalBumpPower;
		end;
		
		if ( material_define.g_SpecularMapEnable == 1 ) then
			t["specularMapPower"] = material_define.g_SpecularMapPower;
		end;
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
			t["AlphaRef"] = material_define.g_AlphaRef/255;
		end;
		
		if ( material_define.g_IsPlants == 1 ) then
			t["g_ScatterColor"] = Ogre.Vector4( material_define.g_ScatterColor.r or 0.0, material_define.g_ScatterColor.g or 0.0, material_define.g_ScatterColor.b or 0.0, 1 );
			t["g_ScatterParams"] = Ogre.Vector4( 1.0, material_define.g_WrapValue or 0.6, material_define.g_ScatterWidth or 0.8, material_define.g_MinValue or 0.0 );
		end;
		
		if(material_define.g_BeastMapEnable==1)then
			t["BeastLightRange"]=Ogre.Vector4(0, 0, 0, 0);
		end;
		
		t["HDRParam"] = Ogre.Vector4( material_define.g_HDRPower or 1.0, 0, 0, 0 );
		
		t["g_StageStyle"] = material_define.g_StageStyle or 1.0;
		t["g_ColourFading"] = Ogre.Vector4(1.0, 1.0, 1.0, 1.0);
		
		return t;
	end;
};

local Program_PS_Pre=
{
	copy_from			= Program_PS_Default;
	source				= "JoyBasicMaterial.ps";
};

local Program_PS_Deffered=
{
	copy_from			= Program_PS_Default;
	source				= "JoyBasicMaterial_DF.ps";
};

local Program_PS_SceneDepth=
{
	copy_from			= Program_PS_Default;
	source				= "JoySceneDepth.ps";
};

local Program_PS_TexDensity=
{
	copy_from			= Program_PS_Default;
	source				= "JoyTexDensity.ps";
};

local Program_PS_Trans=
{
	copy_from			= Program_PS_Default;
	source				= "JoyBasicMaterial.ps";
};

local Program_Ref_PS_Default=
{
	doc	= "pixel program ref";
	
	con	= function()
		if ( context.pixel_program ) then
			return true;
		end;
		return false;
	end;
	
	program_name = function()
		return context.pixel_program_name;
	end;
	
	program_type = Ogre.Global.GPT_FRAGMENT_PROGRAM;
	
	shared_params_ref = function()
			return "EditorControl";
	end;
	
	named_constants=function()
	
		local t={};
		
		if ( material_define.g_NoLighting == 0 ) then
			t["g_LightingParams"]=Ogre.Vector4(material_define.g_Smoothness or 1.0,material_define.g_Shininess or 1.0,100,0.0);
		end;
		
		if(material_define.g_BeastMapEnable==1)then
			t["BeastLightRange"]=Ogre.Vector4(material_define.g_BeastMinValue, material_define.g_BeastMaxValue, 0, 0);
		end;
		
		if ( material_define.g_NormalMapEnable == 1 ) then
			t["normalBumpPower"] = material_define.g_NormalBumpPower;
		end;
		
		if ( material_define.g_SpecularMapEnable == 1 ) then
			t["specularMapPower"] = material_define.g_SpecularMapPower;
		end;
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
			t["AlphaRef"] = material_define.g_AlphaRef/255;
		end;
		
		if ( material_define.g_IsPlants == 1 ) then
			t["g_ScatterColor"] = Ogre.Vector4( material_define.g_ScatterColor.r or 0.0, material_define.g_ScatterColor.g or 0.0, material_define.g_ScatterColor.b or 0.0, 1 );
			t["g_ScatterParams"] = Ogre.Vector4( 1.0, material_define.g_WrapValue or 0.6, material_define.g_ScatterWidth or 0.8, material_define.g_MinValue or 0.0 );
		end;
		
		t["HDRParam"] = Ogre.Vector4( material_define.g_HDRPower or 1.0, 0, 0, 0 );
		t["g_StageStyle"] = material_define.g_StageStyle or 1.0;
		t["g_ColourFading"] = Ogre.Vector4(1.0, 1.0, 1.0, 1.0);
		return t;
	end;
};

JoyBasicMaterial={};

JoyBasicMaterial.createMaterial = Material
{
	name = function() --这个时候context对象是不存在的
		_G.printf("JoyBasicMaterial : %s\n",material_define.MaterialName);
		return material_define.MaterialName;
	end;
	
	after_create = function()
	
		if(material_define.g_UseSkin==1 and material_define.g_BoneNum==nil)then
			material_define.g_BoneNum = 3;
		end;
		
		context.technique_count = 0;
	
	end;
	
	export_file = function()
		--可以返回一个文件名,也可以返回bool
		if
		(
			false
			--or string.find(material_define.MaterialName,'test')
		)
		then
			local name=string.gsub(material_define.MaterialName,"/","_");
			name=string.gsub(name,":","_");
			name=string.gsub(name,"?","_");
			--name="D:\\3D\\0_Trunk\\Fairy_Branch_TLBB\\FairyCore_TLBB\\Media\\MaterialSystem\\Lua\\material_debug\\"..name.."__new_style.txt";
			name="E:\\material_debug\\"..name.."__new_style.txt";
			return name;
		else
			return false;
		end;
	end;
	
	Technique
	{
		doc = "default technique for common render";
		
		con	= function()
			if ( g_IsReleaseMode == 1 ) then
				if ( material_define.g_BeastMapEnable == 1 ) then
					return false;
				else
					return true;
				end;
			else
				return true;
			end;
		end;
		
		after_create = function()
			context.pass_count = 0;
			context.technique_count = context.technique_count+1;
		end;
		
		scheme 	= "default";
		name	= "default";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			con			= function() 
				if ( material_define.g_TwoPass == 1 ) then
					return true;
				else
					return false;
				end;
			end;
			
			alpha_rejection = function()
					return Ogre.Global.CMPF_GREATER_EQUAL,128;
			end;
			
			colour_write = false;
			depth_write = true;
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			
			Program
			{
				copy_from=Program_VS_Default;
				program_name = get_program_name_vs( "default" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Default;
				program_name = get_program_name_ps( "default" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
		
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			depth_write		=	function()
				if ( material_define.g_TwoPass == 1 ) then
					return false;
				else
					if ( material_define.g_ZWriteEnable == 1 ) then
						return true;
					else
						return false;
					end;
				end;
			end;
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse2;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DiffuseBlend;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_SpecularMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_Default;
				program_name = get_program_name_vs( "default" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Default;
				program_name = get_program_name_ps( "default" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};
	
	Technique
	{
		doc = "default technique for common render";

		con	= function()
			if ( Hardware_SM3 == 0 or (g_Project == 0) ) then
				return false;
			end;
			
			if ( material_define.g_AlphaBlendEnable == nil or material_define.g_AlphaBlendEnable == 0 ) then
				return true;
			else
				return false;
			end;
		end;
		
		after_create = function()
			context.pass_count = 0;
			context.technique_count = context.technique_count+1;
		end;
		
		scheme 	= "deffered";
		name	= "deffered";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse2;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DiffuseBlend;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_SpecularMap;
			};	
			TextureUnit
			{
				copy_from=TextureUnit_BeastMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_Deffered;
				program_name = get_program_name_vs( "deffered" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Deffered;
				program_name = get_program_name_ps( "deffered" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};

	Technique
	{
		doc = "default technique for scene depth render";
		
		con	= function()
			return true; --return Effect_ShadowMapSupport == 1;
		end;
		
		after_create = function()
			context.pass_count = 0;
			context.technique_count = context.technique_count+1;
		end;
		
		scheme 	= "sceneDepth";
		name	= "sceneDepth";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			--[[
			alpha_rejection = function()
				if ( material_define.g_AlphaTestEnable==1 ) then
					return Ogre.Global.CMPF_GREATER_EQUAL,material_define.g_AlphaRef;
				end;
			end;
			--]]
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_SceneDepth;
				program_name = get_program_name_vs( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};
			
			Program
			{
				copy_from=Program_PS_SceneDepth;
				program_name = get_program_name_ps( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};
	
	Technique
	{
		doc = "default technique for texture density render";
		
		con	= function()
			return g_IsReleaseMode == 0;
		end;
		
		after_create = function()
			context.pass_count = 0;
			context.technique_count = context.technique_count+1;
		end;
		
		scheme 	= "texDensity";
		name	= "texDensity";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
						
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_TexDensity;
				program_name = get_program_name_vs( "texDensity" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};
			
			Program
			{
				copy_from=Program_PS_TexDensity;
				program_name = get_program_name_ps( "texDensity" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};
	
	Technique
	{
		doc = "default technique for pre render";
		
		con	= function()
			return material_define.g_BeastMapEnable == 1;
		end;
		
		after_create = function()
			context.pass_count = 0;
			context.technique_count = context.technique_count+1;
		end;
		
		scheme 	= "pre";
		name	= "pre";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			--ploygon_mode=Ogre.Global.PM_WIREFRAME;
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse2;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DiffuseBlend;
			};
			TextureUnit
			{
				copy_from=TextureUnit_BeastMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_Pre;
				program_name = get_program_name_vs( "pre" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Pre;
				program_name = get_program_name_ps( "pre" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};
	
	Technique
	{
		doc = "default technique for common render";
		
		con	= function()
			return true;
		end;
		
		after_create = function()
			context.pass_count = 0;
			context.technique_count = context.technique_count+1;
		end;
		
		scheme 	= "forceTrans";
		name	= "forceTrans";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
				doc 		= 	"depth_pass";
				name		= 	"depth_pass";
				copy_from	=	BasePass;
				
				colour_write = false;
				depth_write = true;
				depth_check = true;
				
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_SceneDepth;
				program_name = get_program_name_vs( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};
			
			Program
			{
				copy_from=Program_PS_SceneDepth;
				program_name = get_program_name_ps( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
			
		};
	
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			scene_blend = function()
				return JoyMaterialCommon.BlendMode_DX2Ogre(7),JoyMaterialCommon.BlendMode_DX2Ogre(9);
			end;
			
			depth_write = false;
			depth_check = true;
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse1;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse2;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DiffuseBlend;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_SpecularMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_BeastMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_Trans;
				program_name = get_program_name_vs( "trans" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Trans;
				program_name = get_program_name_ps( "trans" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};
	
};

--[[
local g_material_template =
{
MaterialName				=	'test_material',
g_FogEnable					=	0,
g_AlphaBlendEnable	=	0,
g_AlphaTestEnable		=	1,
g_AlphaRef					=	20,
g_CullMode					=	2,
g_ZEnable						=	1,
g_ZWriteEnable			=	1,
g_ColorWriteEnable	=	1,
g_DepthBiasEnable		=	1,
g_ZFunc							=	4,
g_AlphaFunc					=	5,
g_NoLighting				=	1,
T_D									= "error.dds",
};
JoyBasicMaterial.createMaterial( g_material_template );
--]]