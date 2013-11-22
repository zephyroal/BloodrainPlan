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
local CacheGroup			= OgreMaterial.CacheGroup;
local vs_cache_group	= "basic_material_vs";
local ps_cache_group	= "basic_material_ps";

-- 不同Technique先直接复制，copy_from语义可能有歧义

local TextureUnit_Scene =
{
	copy_from									=	BaseTextureUnit;
	name											=	"scene";
	con												=	true;
	sampler_register_name			=	"SS";
	texture										= "error.dds";
	tex_address_mode					=	getTextureAddressMode("T_D_Address_Mode","T_D_Address_Mode");
	filtering 								= getPointFilter();
	type											=	Ogre.Global.TEX_TYPE_2D;
	content_type							= "compositor";
	compositorName						= "Main";
	compositorTextureName			= "main_buffer";
	compositorTextureIndex		= 0;
};

local TextureUnit_Normal =
{
	copy_from									=	BaseTextureUnit;
	name											=	"normal";
	con												=	true;
	sampler_register_name			=	"SN";
	texture										= "error.dds";
	tex_address_mode					=	getTextureAddressMode("T_D_Address_Mode","T_D_Address_Mode");
	filtering 								= getPointFilter();
	type											=	Ogre.Global.TEX_TYPE_2D;
	content_type							= "compositor";
	compositorName						= "Main";
	compositorTextureName			= "main_buffer";
	compositorTextureIndex		= 1;
};

local TextureUnit_Ambient =
{
	copy_from									=	BaseTextureUnit;
	name											=	"ambient";
	con												=	context_bool( "g_LightType", Ogre.Global.LT_Ambient );
	sampler_register_name			=	"SA";
	texture										= "error.dds";
	tex_address_mode					=	getTextureAddressMode("T_D_Address_Mode","T_D_Address_Mode");
	filtering 								= getPointFilter();
	type											=	Ogre.Global.TEX_TYPE_2D;
	content_type							= "compositor";
	compositorName						= "Main";
	compositorTextureName			= "main_buffer";
	compositorTextureIndex		= 2;
};

local TextureUnit_OCC 	=
{
	copy_from									=	BaseTextureUnit;
	name											=	"occ";
	con												=	context_bool( "g_LightType", Ogre.Global.LT_Ambient );
	sampler_register_name			=	"SO";
	texture										= "error.dds";
	tex_address_mode					=	getTextureAddressMode("T_D_Address_Mode","T_D_Address_Mode");
	filtering 								= getPointFilter();
	type											=	Ogre.Global.TEX_TYPE_2D;
	content_type							= "compositor";
	compositorName						= "Main";
	compositorTextureName			= "ambient_buffer";
	compositorTextureIndex		= 0;
};

local TextureUnit_RandomN 	=
{
	copy_from									=	BaseTextureUnit;
	name											=	"randomn";
	con												=	context_bool( "g_LightEffect", 14 );
	sampler_register_name			=	"SRN";
	texture										= "waveNM.png";
	tex_address_mode					=	"wrap";
	type											=	Ogre.Global.TEX_TYPE_2D;
};

local TextureUnit_SpotProject 	=
{
	copy_from									=	BaseTextureUnit;
	name											=	"spotProject";
	con												=	context_bool( "g_SpotProjTexEnable", 1 );
	sampler_register_name			=	"SPT";
	texture									=  function() return material_define["T_D_SpotProjectTexture"] end;
	scroll_anim				= getScrollAnim( "T_D_ScrollAnim_Enable", "T_D_Anim_X", "T_D_Anim_Y", "T_D_FixedTransform" );
	rotate_anim				= getRotateAnim( "T_D_RotateAnim_Enable", "T_D_Anim_Rotate", "T_D_FixedTransform" );
	scale_anim				= getScaleAnim( "T_D_ScaleAnim_Enable", "T_D_Scale_X", "T_D_Scale_Y", "T_D_FixedTransform" );
	tex_address_mode					=	"wrap";
	type											=	Ogre.Global.TEX_TYPE_2D;
};

--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
local Program_VS_Deffered=
{
	cache_group 	= vs_cache_group;
	program_type	= Ogre.Global.GPT_VERTEX_PROGRAM;
	source				= "JoyLightMaterial.vs";
	entry_point		= "main";
	language			= "hlsl";
	target				= "vs_3_0";
	
	column_major_matrices	= false;
	preprocessor_defines	= function() return context.vertex_program_define; end;
	
	program_name = function()
	
		local names = {};
	  
		material_define.g_NoLighting = 1;
		names.g_NoLighting = "NL";
		
		names.g_LightType = "LT";
		names.g_LightEffect = "LE";
		
		names.g_LightState = "LS";
		
		local temp1	=	{"JBLVS0/";};
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
	
	named_auto_constants=
	{
		{"world_matrix"};
		{"worldview_matrix"};
		{"inverse_world_matrix"};
		{"inverse_transpose_world_matrix"};
		{"worldviewproj_matrix"};
		{"inverse_projection_matrix"};
		{"camera_position"};
		{"camera_position_object_space"};
		{"light_position"};
		{"fog_colour"};
		{"fog_params"};
		{"world_matrix_array_3x4",Ogre.Global.ACT_WORLD_MATRIX_ARRAY_3x4, 68};
		{"view_matrix"};
		{"projection_matrix"};
		{"texture_matrix_0",Ogre.Global.ACT_TEXTURE_MATRIX, 0};
	};
	
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
		
		t["g_Atten"] = Ogre.Vector4( material_define.g_Atten.x, material_define.g_Atten.y, material_define.g_Atten.z, material_define.g_Atten.w );
		t["g_Direction"] = Ogre.Vector4( material_define.g_Direction.x, material_define.g_Direction.y, material_define.g_Direction.z, material_define.g_Direction.w );
		t["g_Position"] = Ogre.Vector4( material_define.g_Position.x, material_define.g_Position.y, material_define.g_Position.z, material_define.g_Position.w );
			
		return t;
	end;
};

local Program_VS_SceneDepth=
{
	cache_group 	= vs_cache_group;
	program_type	= Ogre.Global.GPT_VERTEX_PROGRAM;
	source			= "JoySceneDepth.vs";
	entry_point		= "main";
	language		= "hlsl";
	target			= "vs_3_0";
	
	column_major_matrices	= false;
	preprocessor_defines	= function() return context.vertex_program_define; end;
	
	program_name = function()
	
		local names = {};
		
		local temp1	=	{"JLMVS1/";};
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
	
	named_auto_constants=
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
		{"viewproj_matrix"};
		{"texture_matrix_0",Ogre.Global.ACT_TEXTURE_MATRIX, 0};
		{"texture_matrix_1",Ogre.Global.ACT_TEXTURE_MATRIX, 1};	};
	
	named_auto_constants_real=
	{
		{"time"},
		{"time_0_1",Ogre.Global.ACT_TIME_0_1,1000},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants = function()
		local t = {};

		return t;
	end;
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
		return t;
	end;
	
	named_constants = function()
		local t={};
		
		return t;
	end;
}; 

local Program_PS_Deffered=
{
	doc				= "pixel program";
	cache_group 	= ps_cache_group;
	program_type	= Ogre.Global.GPT_FRAGMENT_PROGRAM;
	source			= "JoyLightMaterial.ps";
	entry_point		= "main";
	language		= "hlsl";
	target			= "ps_3_0";
	
	column_major_matrices	=	true;
	preprocessor_defines	=	function() return context.pixel_program_define; end;
	
	program_name = function()
	
		local names = {};
		
		material_define.g_NoLighting = 1;
		names.g_NoLighting = "NL";
		
		names.g_LightType = "LT";
		names.g_LightEffect = "LE";	
		names.g_LightState = "LS";	
		names.g_SpotProjTexEnable = "SP";
		
		local temp1={"JLMPS0/";};
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
		{"view_matrix"};
		{"inverse_view_matrix"};
		{"inverse_transpose_world_matrix"};
		{"inverse_texture_size",Ogre.Global.ACT_INVERSE_TEXTURE_SIZE,1};
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
		
		{"texture_matrix_0",Ogre.Global.ACT_TEXTURE_MATRIX, 0};
	};
	
	named_auto_constants_real=
	{
		{"time"},
		{"pass_number"},
		{"far_clip_distance"},
		{"light_count"},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants=function()
		local t={};
		
			t["g_Diffuse"] = Ogre.Vector4( material_define.g_Diffuse.r, material_define.g_Diffuse.g, material_define.g_Diffuse.b, material_define.g_Diffuse.a );
			t["g_Specular"] = Ogre.Vector4( material_define.g_Specular.r, material_define.g_Specular.g, material_define.g_Specular.b, material_define.g_Specular.a );
			t["g_Atten"] = Ogre.Vector4( material_define.g_Atten.x, material_define.g_Atten.y, material_define.g_Atten.z, material_define.g_Atten.w );
			t["g_Direction"] = Ogre.Vector4( material_define.g_Direction.x, material_define.g_Direction.y, material_define.g_Direction.z, material_define.g_Direction.w );
			t["g_Position"] = Ogre.Vector4( material_define.g_Position.x, material_define.g_Position.y, material_define.g_Position.z, material_define.g_Position.w );
			t["g_PowerScale"] = material_define.g_PowerScale;	
			
		if ( material_define.g_LightType == Ogre.Global.LT_SPOTLIGHT ) then
			
			t["g_Spot"] = Ogre.Vector4(material_define.g_Spot.x or 0.0, material_define.g_Spot.y or 0.0, material_define.g_Spot.z or 0.0, 0.0);
			
			if( material_define.g_SpotProjTexEnable == 1 ) then
				
				t["g_SpotWvpMatrix0"] = Ogre.Vector4(material_define.g_SpotWvpMatrix0.x or 1.0, material_define.g_SpotWvpMatrix0.y or 0.0, material_define.g_SpotWvpMatrix0.z or 0.0, material_define.g_SpotWvpMatrix0.w or 0.0 );
				t["g_SpotWvpMatrix1"] = Ogre.Vector4(material_define.g_SpotWvpMatrix1.x or 0.0, material_define.g_SpotWvpMatrix1.y or 1.0, material_define.g_SpotWvpMatrix1.z or 0.0, material_define.g_SpotWvpMatrix1.w or 0.0 );
				t["g_SpotWvpMatrix2"] = Ogre.Vector4(material_define.g_SpotWvpMatrix2.x or 0.0, material_define.g_SpotWvpMatrix2.y or 0.0, material_define.g_SpotWvpMatrix2.z or 1.0, material_define.g_SpotWvpMatrix2.w or 0.0 );
				t["g_SpotWvpMatrix3"] = Ogre.Vector4(material_define.g_SpotWvpMatrix3.x or 0.0, material_define.g_SpotWvpMatrix3.y or 0.0, material_define.g_SpotWvpMatrix3.z or 0.0, material_define.g_SpotWvpMatrix3.w or 1.0 );
			
			end;
			
		end;
			
		return t;
	end;
};

local Program_PS_SceneDepth=
{
	doc				= "pixel program";
	cache_group 	= ps_cache_group;
	program_type	= Ogre.Global.GPT_FRAGMENT_PROGRAM;
	source			= "JoySceneDepth.ps";
	entry_point		= "main";
	language		= "hlsl";
	target			= "ps_3_0";
	
	column_major_matrices	=	true;
	preprocessor_defines	=	function() return context.pixel_program_define; end;
	
	program_name = function()
	
		local names = {};
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
		   names.g_AlphaTestEnable = "AT";
		end;
		
		if ( material_define.g_ZWriteEnable == 1 ) then
			names.g_ZWriteEnable = "ZW";
		end;
		
		local temp1={"JLMPS3/";};
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
		
		return t;
	end;
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
		
			t["g_Diffuse"] = Ogre.Vector4( material_define.g_Diffuse.r, material_define.g_Diffuse.g, material_define.g_Diffuse.b, material_define.g_Diffuse.a );
			t["g_Specular"] = Ogre.Vector4( material_define.g_Specular.r, material_define.g_Specular.g, material_define.g_Specular.b, material_define.g_Specular.a );
			t["g_Atten"] = Ogre.Vector4( material_define.g_Atten.x, material_define.g_Atten.y, material_define.g_Atten.z, material_define.g_Atten.w );
			t["g_Direction"] = Ogre.Vector4( material_define.g_Direction.x, material_define.g_Direction.y, material_define.g_Direction.z, material_define.g_Direction.w );
			t["g_Position"] = Ogre.Vector4( material_define.g_Position.x, material_define.g_Position.y, material_define.g_Position.z, material_define.g_Position.w );
			t["g_PowerScale"] = material_define.g_PowerScale;
			
		if ( material_define.g_LightType == Ogre.Global.LT_SPOTLIGHT ) then
		
			t["g_Spot"] = Ogre.Vector4(material_define.g_Spot.x or 0.0, material_define.g_Spot.y or 0.0, material_define.g_Spot.z or 0.0, 0.0);
			
			if( material_define.g_SpotProjTexEnable == 1 ) then
				
				t["g_SpotWvpMatrix0"] = Ogre.Vector4(material_define.g_SpotWvpMatrix0.x or 1.0, material_define.g_SpotWvpMatrix0.y or 0.0, material_define.g_SpotWvpMatrix0.z or 0.0, material_define.g_SpotWvpMatrix0.w or 0.0 );
				t["g_SpotWvpMatrix1"] = Ogre.Vector4(material_define.g_SpotWvpMatrix1.x or 0.0, material_define.g_SpotWvpMatrix1.y or 1.0, material_define.g_SpotWvpMatrix1.z or 0.0, material_define.g_SpotWvpMatrix1.w or 0.0 );
				t["g_SpotWvpMatrix2"] = Ogre.Vector4(material_define.g_SpotWvpMatrix2.x or 0.0, material_define.g_SpotWvpMatrix2.y or 0.0, material_define.g_SpotWvpMatrix2.z or 1.0, material_define.g_SpotWvpMatrix2.w or 0.0 );
				t["g_SpotWvpMatrix3"] = Ogre.Vector4(material_define.g_SpotWvpMatrix3.x or 0.0, material_define.g_SpotWvpMatrix3.y or 0.0, material_define.g_SpotWvpMatrix3.z or 0.0, material_define.g_SpotWvpMatrix3.w or 1.0 );
			
			end;
			
		end;
			
		return t;
	end;
};

JoyLightMaterial={};

JoyLightMaterial.createMaterial = Material
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
			doc 			= 	"base_pass";
			name			= 	"diffuse_pass";
			copy_from	=	BasePass;
			
			scene_blend = function()
				if ( material_define.g_LightType == Ogre.Global.LT_Ambient ) then						-- ambient light
					return "replace";	
				else
					return "add";																					-- others
				end;
			end;
			
			depth_check = true;
			depth_write = false;

			TextureUnit
			{
				copy_from=TextureUnit_SpotProject;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Scene;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Ambient;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Normal;
			};
			TextureUnit
			{
				copy_from=TextureUnit_OCC;
			};
			TextureUnit
			{
				copy_from=TextureUnit_LightModel;
			};
			TextureUnit
			{
				copy_from=TextureUnit_RandomN;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_Deffered;
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_Deffered;
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
		
		scheme 	= "ccw_depth";
		name	= "ccw_depth";
		
		function()
			local tec=context.technique;
		end;
		
		Pass
		{
			doc 			= 	"ccw_depth_pass";
			name			= 	"ccw_depth";
			copy_from	=	BasePass;
			
			scene_blend = function()
					return "replace";	
			end;
			
			depth_check = false;
			depth_write = false;
			cull_hardware = Ogre.Global.CULL_ANTICLOCKWISE;
			cull_software = Ogre.Global.MANUAL_CULL_FRONT;
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_SceneDepth;
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS_Default;
			};

			Program
			{
				copy_from=Program_PS_SceneDepth;
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS_Default;
			};
		};
	};
	
};