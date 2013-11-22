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
local CacheGroup		= OgreMaterial.CacheGroup;
local vs_cache_group= "particle_material_vs";
local ps_cache_group= "particle_material_ps";

local TextureUnit_Diffuse =
{
	copy_from				=	BaseTextureUnit;
	name						=	"diffuse1";
	con							=	true;
	sampler_register_name	=	"SD";
	texture 					= getUnAnimationTexture	("ForceEnable","T_D");
	tex_address_mode	=	getTextureAddressMode("T_D_Address_Mode","T_D_Address_Mode");
	-- filtering 				= getDiffuseFilter();
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

local TextureUnit_Distortion = 
{
    copy_from               =  BaseTextureUnit;
	name                       =   "distortion";
	con                        =    any_one( context_bool("g_SpaceDistortionEnable", 1), context_bool("g_TexDistortionEnable", 1) );
	sampler_register_name   =  "SDT";
	texture                    =   getUnAnimationTexture    ("ForceEnable", "T_TD");
	tex_address_mode        =  getTextureAddressMode("T_TD_Address_Mode", "T_TD_Address_Mode");
  scroll_anim				= getScrollAnim( "T_TD_ScrollAnim_Enable", "T_TD_Anim_X", "T_TD_Anim_Y", "T_TD_FixedTransform" );
	rotate_anim				= getRotateAnim( "T_TD_RotateAnim_Enable", "T_TD_Anim_Rotate", "T_TD_FixedTransform" );
	scale_anim				= getScaleAnim( "T_TD_ScaleAnim_Enable", "T_TD_Scale_X", "T_TD_Scale_Y", "T_TD_FixedTransform" );
	-- filtering 				= 	getDiffuseFilter();
	type                    =  Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_Scene = 
{
	copy_from                =  BaseTextureUnit;
	name                     =  "scene";
	con                      = 	function() 
									if ( (material_define.g_SpaceDistortionEnable == 1) and (Hardware_SM3==1) ) then
										return true
									else
										return false
									end;
								end;
	sampler_register_name    =  "SDS";
	texture                  =  "error.dds";
	tex_address_mode         =  getTextureAddressMode("g_Diffuse1_AddressU", "g_Diffuse1_AddressV");
	--filtering              =  getNormalFilter();
	type                     =  Ogre.Global.TEX_TYPE_2D;
	content_type             =  "compositor";
	compositorName           =  "Main";
	compositorTextureName    =  "swap_buffer";
	compositorTextureIndex    =  0;
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

--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
local Program_VS_Default=
{
	cache_group 	= vs_cache_group;
	program_type	= Ogre.Global.GPT_VERTEX_PROGRAM;
	source				= "JoyParticleMaterial.vs";
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
	
	program_name = function()
	
		local names = {};
	  
		material_define.g_IsReleaseMode = g_IsReleaseMode or 0;
		names.g_IsReleaseMode = "RM";
	  
	  if ( material_define.g_Diffuse2Enable == 1 ) then
		  names.g_Diffuse2Enable = "D2E";
		end;

		if( material_define.g_TexDistortionEnable == 1 ) then
		  names.g_TexDistortionEnable = "TDE";
		end;
		
		if( material_define.g_SpaceDistortionEnable == 1 and (Hardware_SM3==1) ) then
		  names.g_SpaceDistortionEnable = "SDE";
		end;
		
		if ( material_define.g_Diffuse2BlendEnable == 1 ) then
		  names.g_Diffuse2BlendEnable = "D2BE";
		  names.g_Diffuse2BlendType = "D2BT";
		end;
		
		if ( material_define.T_D_ScrollAnim_Enable == 1 or material_define.T_D_RotateAnim_Enable == 1 or material_define.T_D_ScaleAnim_Enable == 1 ) then
			material_define.g_Texture1EffectEnable = 1;
			names.g_Texture1EffectEnable = "T1EE";
		end;
		
		if ( material_define.T_D2_ScrollAnim_Enable == 1 or material_define.T_D2_RotateAnim_Enable == 1 or material_define.T_D2_ScaleAnim_Enable == 1 ) then
			material_define.g_Texture2EffectEnable = 1;
			names.g_Texture2EffectEnable = "T2EE";
		end;
		
		if ( Hardware_SM3 == 1 ) then
	  	material_define.g_SM3 = 1;
	  	names.g_SM3 = "SM3";
	  end;
		
		local temp1	=	{"JPMVS0/";};
		local temp2	=	{"__NO_DEBUG" };
		for k,v in pairs(names) do
			temp1[#temp1+1]=";"..v.."="..tostring((material_define[k] or 0));
			temp2[#temp2+1]=";"..k.."="..tostring((material_define[k] or 0));
		end;
		
		--生成TextureUnit索引
		--TU_D2,TU_LMT,TU_N,TU_BB
		local texture_unit = 1;
		if ( material_define.g_Diffuse2Enable == 1 and material_define.g_Diffuse2UseSelfUV == 1 )then
			temp2[#temp2+1] = ";TU_D2=" .. tostring(texture_unit);
			texture_unit = texture_unit+1;
		end;
		if( material_define.g_LightMapEnable==1 and material_define.g_LightMapUseSelfUV==1)then
			temp2[#temp2+1]=";TU_LMT="..tostring(texture_unit);
			texture_unit=texture_unit+1;
		end;		
		if( material_define.g_NormalEnable==1 and material_define.g_NormalUseSelfUV==1)then
			temp2[#temp2+1]=";TU_N="..tostring(texture_unit);
			texture_unit=texture_unit+1;
		end;
		if( material_define.g_BillboardEnable==1)then
			temp2[#temp2+1]=";TU_BB="..tostring(texture_unit);
			texture_unit=texture_unit+1;
		end;
		
		local v1 = table.concat(temp1);
		local v2 = table.concat(temp2);
		context.vertex_program_name  	= v1;
		context.vertex_program_define	= v2;
		
		return v1;
	
	end;
	
	named_auto_constants=function()
	
	local t =
	{
		{"world_matrix"};
		{"worldview_matrix"};
		{"inverse_transpose_world_matrix"};
		{"worldviewproj_matrix"};
		{"camera_position"};
		{"light_position"};
		{"fog_colour"};
		{"fog_params"};
		{"world_matrix_array_3x4",Ogre.Global.ACT_WORLD_MATRIX_ARRAY_3x4, 68};
		{"view_matrix"};
		{"projection_matrix"};
		{"texture_matrix_0",Ogre.Global.ACT_TEXTURE_MATRIX, 0};
		{"texture_matrix_1",Ogre.Global.ACT_TEXTURE_MATRIX, 1};
		--{"texture_matrix_2",Ogre.Global.ACT_TEXTURE_MATRIX, 12};
	};
	
	if ( material_define.g_TexDistortionEnable == 1 or material_define.g_SpaceDistortionEnable == 1) then
	  table.insert(t,{"texture_matrix_2",Ogre.Global.ACT_TEXTURE_MATRIX,2}); 
	else
	  table.insert(t,{"texture_matrix_2",Ogre.Global.ACT_TEXTURE_MATRIX,1});
	end;

	return t;
	end;
	
	named_auto_constants_real=
	{
		{"time"},
		{"time_0_1",Ogre.Global.ACT_TIME_0_1,1000},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants = function()
		local t = {};

		if ( material_define.g_BillboardGrassWindEnable == 1 ) then
			t["g_BillboardGrassWindInfo"] = Ogre.Vector4(material_define.g_BillboardGrassWindPower or 0.0,material_define.g_BillboardGrassWindFreq or 0.0,material_define.g_BillboardGrassWindCycle or 0.0,0.0);
		end;
		
		t["use_instance_rendering"] = Ogre.Vector4(0,0,0,0);
		
		return t;
	end;
};

local Program_VS_TexDensity=
{
	copy_from			= Program_VS_Default;
	source			= "JoyTexDensity.vs";
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
		if(material_define.g_WindEnable==1)then
			table.insert(t,"WindParameters");
		end;
		return t;
	end;
	
	named_constants = function()
		local t={};
		if ( material_define.g_Texture1LayerAnimationEnable ) then
		  t["g_Texture1LayerAnimationSpeed"]=material_define.g_Texture1LayerAnimationSpeed or 0;
		end;
		
		if ( material_define.g_UseSkin == 1 ) then
			t["g_BoneNum"] = Ogre.Vector4( material_define.g_WeightPerBone or 1,0,0,0 );
		end;
		
		t["use_instance_rendering"] = Ogre.Vector4(0,0,0,0);
		
		return t;
	end;
}; 

local Program_PS_Default=
{
	doc				= "pixel program";
	cache_group 	= ps_cache_group;
	program_type	= Ogre.Global.GPT_FRAGMENT_PROGRAM;
	source			= "JoyParticleMaterial.ps";
	entry_point		= "main";
	language		= "hlsl";
	target			= function() 
										if ( Hardware_SM3 == 1 ) then
											return "ps_3_0";
										else
											return "ps_2_0";
										end;
									end;
	
	column_major_matrices	=	true;
	preprocessor_defines	=	function() return context.pixel_program_define; end;
	
	program_name = function()
	
		local names = {};
		
		material_define.g_IsReleaseMode = g_IsReleaseMode or 0;
		names.g_IsReleaseMode = "RM";
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
		   names.g_AlphaTestEnable = "AT";
		end;
		
		if ( material_define.g_FogEnable == 1 ) then
			names.g_FogEnable = "FE";
		end;
		
		if ( Hardware_SM3 == 1 ) then
	  	material_define.g_SM3 = 1;
	  	names.g_SM3 = "SM3";
	    end;
		
		mdefs.g_CapsLevel = Hardware_CapsLevel;
		names.g_CapsLevel = "CL";

	  if( material_define.g_TexDistortionEnable == 1 ) then
		  names.g_TexDistortionEnable = "TDE";
		end;
		
	  if( material_define.g_SpaceDistortionEnable == 1 and (Hardware_SM3==1) ) then
		  names.g_SpaceDistortionEnable = "SDE";
		end;
		
		if ( material_define.g_Diffuse2Enable == 1 ) then
		  names.g_Diffuse2Enable = "D2E";
		end;
		
		if ( material_define.g_Diffuse2BlendEnable == 1 ) then
		  names.g_Diffuse2BlendEnable = "D2BE";
		  names.g_Diffuse2BlendType = "D2BT";
		end;
		
		local temp1={"JPMPS0/";};
		local temp2={"__NO_DEBUG" };
		for k,v in pairs(names) do
			temp1[#temp1+1]=";"..v.."="..tostring((material_define[k] or 0));
			temp2[#temp2+1]=";"..k.."="..tostring((material_define[k] or 0));
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
	
	named_auto_constants=
	{
		{"world_matrix"};
		{"inverse_transpose_world_matrix"};
		{"worldviewproj_matrix"};
		{"ambient_light_colour"};
		{"surface_diffuse_colour"};
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
		{"light_position_object_space"};
		{"light_position_view_space"};

		{"camera_position"};
		{"camera_position_object_space"};
		
		{"surface_specular_colour"};
		{"surface_shininess"};

		{"surface_diffuse_colour_per_object",Ogre.Global.ACT_CUSTOM,0};
		{"time_in_effect",Ogre.Global.ACT_CUSTOM,1};
	};
	
	named_auto_constants_real=
	{
		{"time"},
		{"light_count"},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants=function()
		local t={};
	
		if ( material_define.g_AlphaTestEnable == 1 ) then
			t["AlphaRef"] = material_define.g_AlphaRef/255;
		end;
		
		t["HDRParam"] = Ogre.Vector4( material_define.g_HDRPower or 1.0, 0, 0, 0 );

		t["g_StageStyle"] = material_define.g_StageStyle or 1.0;

		t["g_ColourFading"] = Ogre.Vector4(1.0, 1.0, 1.0, 1.0);
		
		t["DistortionPower"] = material_define.g_DistortionPower or 1.0;

		
		return t;
	end;
};

local Program_PS_TexDensity=
{
	copy_from			= Program_PS_Default;
	source				= "JoyTexDensity.ps";
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
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
			t["AlphaRef"] = material_define.g_AlphaRef/255;
		end;
		
		t["HDRParam"] = Ogre.Vector4( material_define.g_HDRPower or 1.0, 0, 0, 0 );

		t["g_StageStyle"] = material_define.g_StageStyle or 1.0;

		t["g_ColourFading"] = Ogre.Vector4(1.0, 1.0, 1.0, 1.0);
		
		t["DistortionPower"] = material_define.g_DistortionPower or 1.0;

		
		return t;
	end;
};

JoyParticleMaterial={};

JoyParticleMaterial.createMaterial = Material
{
	name = function() --这个时候context对象是不存在的
		_G.printf("JoyParticleMaterial : %s\n",material_define.MaterialName);
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
	
	-- todo : qtx lod for particles
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
			depth_write = false;
			
			--ploygon_mode=Ogre.Global.PM_WIREFRAME;
			
			TextureUnit
			{
				copy_from=TextureUnit_Diffuse;
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
				copy_from=TextureUnit_Distortion;
			};
			TextureUnit
			{
			  copy_from=TextureUnit_Scene;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_Default;
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Default;
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
};