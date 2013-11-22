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
local vs_cache_group= "terrain_material_vs";
local ps_cache_group= "terrain_material_ps";

local TextureUnit_BottomLayer =
{
	copy_from							=	BaseTextureUnit;
	name									=	"BottomLayer";
	con										=	context_bool("g_BottomLayerEnable",1);
	sampler_register_name	=	"SL1";
	texture 							= getUnAnimationTexture	("ForceEnable","T_BottomLayer");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode 			= "wrap";
};

local TextureUnit_MiddleLayer =
{
	copy_from							=	BaseTextureUnit;
	name									=	"MiddleLayer";
	con										=	context_bool("g_MiddleLayerEnable",1);
	sampler_register_name	=	"SL2";
	texture 							= getUnAnimationTexture	("ForceEnable","T_MiddleLayer");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode		  = "wrap";
};

local TextureUnit_UpperLayer =
{
	copy_from							=	BaseTextureUnit;
	name									=	"UpperLayer";
	con										=	context_bool("g_UpperLayerEnable",1);
	sampler_register_name	=	"SL3";
	texture 							= getUnAnimationTexture	("ForceEnable","T_UpperLayer");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode		  = "wrap";
};

local TextureUnit_DetailLayer =
{
	copy_from							=	BaseTextureUnit;
	name									=	"DetailLayer";
	con										=	context_bool("g_DetailLayerEnable",1);
	sampler_register_name	=	"SLDetail";
	texture 							= getUnAnimationTexture	("ForceEnable","T_DetailLayer");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode		  = "clamp";
};

local TextureUnit_NormalLayer =
{
	copy_from							=	BaseTextureUnit;
	name									=	"NormalLayer";
	con										=	context_bool("g_NormalLayerEnable",1);
	sampler_register_name	=	"SLNormal";
	texture 							= getUnAnimationTexture	("ForceEnable","T_NormalLayer");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode		  = "clamp";
};

local TextureUnit_BlendLayer =
{
	copy_from							=	BaseTextureUnit;
	name									=	"BlendLayer";
	con										=	context_bool("g_BlendLayerEnable",1);
	sampler_register_name	=	"SLBlend";
	texture 							= getUnAnimationTexture	("ForceEnable","T_BlendLayer");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode		  = "clamp";
};

local TextureUnit_LightMap_Beast =
{
	copy_from							=	BaseTextureUnit;
	name									=	"lightmap_beast";
	con										=	context_bool("g_LightMapEnable",1);
	sampler_register_name	=	"SBM";
	texture 							= getUnAnimationTexture	("ForceEnable","T_LightMap");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode 			= "clamp";
};

local TextureUnit_ColorMap =
{
	copy_from							=	BaseTextureUnit;
	name									=	"colormap";
	con										=	context_bool("g_ColorMapEnable",1);
	sampler_register_name	=	"SC";
	texture 							= getUnAnimationTexture	("ForceEnable","T_ColorMap");
	-- filtering 						= getDiffuseFilter();
	type									=	Ogre.Global.TEX_TYPE_2D;
	tex_address_mode 			= "clamp";
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
	  	mdefs.g_LightMapEnable = 0;
	  end;
	  
	  mdefs.g_IsReleaseMode = g_IsReleaseMode or 0;
	  names.g_IsReleaseMode = "RM";
		
		 -- 最大支持的光源数量
	  mdefs.MAX_LIGHT_NUM = Max_Light_Num or 8;
	  names.MAX_LIGHT_NUM = "MLT";
		
		names.g_BottomLayerEnable = "L1N";
		
		if ( mdefs.g_MiddleLayerEnable == 1 ) then
			names.g_MiddleLayerEnable = "L2N";
		end;
		
		if ( Hardware_SM3 == 1 ) then
			mdefs.g_SM3 = 1;
	  	names.g_SM3 = "SM3";
	  end;
		
		if ( mdefs.g_UpperLayerEnable == 1 ) then
			names.g_UpperLayerEnable = "L3N";
		end;
		
		if ( mdefs.g_DetailLayerEnable == 1 ) then
			names.g_DetailLayerEnable = "LDetailN";
		end;
		
		if ( mdefs.g_NormalLayerEnable == 1 ) then
			names.g_NormalLayerEnable = "LNormalN";
		end;
		
		if ( mdefs.g_BlendLayerEnable == 1 ) then
			names.g_BlendLayerEnable = "LBlendN";
		end;
		
		if ( mdefs.g_ColorMapEnable == 1 ) then
			names.g_ColorMapEnable = "LCN";
		end;
		
		if ( mdefs.g_LightMapEnable == 1 ) then
			names.g_LightMapEnable = "LMN";
		end;
		
		local temp1	=	{ "JTMVS_" .. tostring(shading_mode); };
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
		
		local names = {};
		
		_G.Utility.CopyTable( material_define, mdefs );
		
		-- 提前设定不同模式下的宏切换
		if ( shading_mode ~= "default" ) then
	  	mdefs.g_NoLighting = 1;
	  else
	  	mdefs.g_LightMapEnable = 0;
	  end;
	  
	  mdefs.g_IsReleaseMode = g_IsReleaseMode or 0;
	  names.g_IsReleaseMode = "RM";
	  
		 -- 最大支持的光源数量
	  mdefs.MAX_LIGHT_NUM = Max_Light_Num or 8;
	  names.MAX_LIGHT_NUM = "MLT";
		
		if ( mdefs.g_NoLighting == 1 ) then
		   names.g_NoLighting = "NL";
		end;
		
		if ( Hardware_SM3 == 1 ) then
			mdefs.g_SM3 = 1;
	  	names.g_SM3 = "SM3";
		end;
		
		mdefs.g_CapsLevel = Hardware_CapsLevel;
		names.g_CapsLevel = "CL";
		
		if ( mdefs.g_FogEnable == 1 ) then
			names.g_FogEnable = "FE";
		end;
		
		names.g_BottomLayerEnable = "L1N";
		
		if ( mdefs.g_MiddleLayerEnable == 1 ) then
			names.g_MiddleLayerEnable = "L2N";
		end;
		
		if ( mdefs.g_UpperLayerEnable == 1 ) then
			names.g_UpperLayerEnable = "L3N";
		end;
		
		if ( mdefs.g_OldVersionEnable == 1) then
			names.g_OldVersionEnable = "Old";
		end;
		
		if ( mdefs.g_DetailLayerEnable == 1 ) then
			names.g_DetailLayerEnable = "LDetailN";
		end;
		
		if ( mdefs.g_NormalLayerEnable == 1 ) then
			names.g_NormalLayerEnable = "LNormalN";
		end;
		
		if ( mdefs.g_BlendLayerEnable == 1 ) then
			names.g_BlendLayerEnable = "LBlendN";
		end;
		
		if ( mdefs.g_ColorMapEnable == 1 ) then
			names.g_ColorMapEnable = "LCN";
		end;
		
		if ( mdefs.g_LightMapEnable == 1 ) then
			names.g_LightMapEnable = "LMN";
			names.g_BeastMapEnable = "LBS";
		end;
			
		local temp1={ "JTMPS_" .. tostring(shading_mode); };
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

--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
local Program_VS_Default=
{
	cache_group 	= vs_cache_group;
	program_type	= Ogre.Global.GPT_VERTEX_PROGRAM;
	source				= "JoyTerrainMaterial.vs";
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
	
	named_auto_constants=
	{
		{"world_matrix"};
		{"view_matrix"};
		{"projection_matrix"};
		{"worldview_matrix"};
		{"inverse_transpose_world_matrix"};
		{"worldviewproj_matrix"};
		{"camera_position"};
		{"light_position"};
		{"fog_colour"};
		{"fog_params"};
	};
	
	named_auto_constants_real=
	{
		{"time"},
		{"time_0_x",Ogre.Global.ACT_TIME_0_X,1000},
	};
	
	named_constants = function()
		local t = {};

		if ( material_define.g_BillboardGrassWindEnable == 1 ) then
			t["g_BillboardGrassWindInfo"]=Ogre.Vector4(material_define.g_BillboardGrassWindPower or 0.0,material_define.g_BillboardGrassWindFreq or 0.0,material_define.g_BillboardGrassWindCycle or 0.0,0.0);
		end;

		return t;
	end;
};

local Program_VS_Pre=
{
	copy_from			= Program_VS_Default;
	source				= "JoyTerrainMaterial.vs";
};

local Program_VS_Deffered=
{
	copy_from			= Program_VS_Default;
	source				= "JoyTerrainMaterial_DF.vs";
};

local Program_VS_SceneDepth=
{
	copy_from			= Program_VS_Default;
	source				= "JoySceneDepth.vs";
};

local Program_VS_TexDensity=
{
	copy_from			= Program_VS_Default;
	source			= "JoyTexDensity.vs";
};

local Program_Ref_VS=
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

		return t;
	end;
};

local Program_PS_Default=
{
	doc						= "pixel program";
	cache_group 	= ps_cache_group;
	program_type	= Ogre.Global.GPT_FRAGMENT_PROGRAM;
	source				= "JoyTerrainMaterial.ps";
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
		{"surface_emissive_colour"};
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
		
		if ( material_define.g_NoLighting == 0 ) then
			t["g_LightingParams"]=Ogre.Vector4(material_define.g_Smoothness or 1.0,material_define.g_Shininess or 1.0,100,0.0);
		end;
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
			t["AlphaRef"] = material_define.g_AlphaRef/255;
		end;
		
		if ( material_define.g_LightMapEnable == 1) then
			t["BeastLightRange"] = Ogre.Vector4( 0.0 , 1.0 , 0.0 , 0.0);
		end
		
		t["HDRParam"] = Ogre.Vector4( material_define.g_HDRPower or 1.0, 0, 0, 0 );
		
		return t;
	end;
};

local Program_PS_Pre=
{
	copy_from			= Program_PS_Default;
	source				= "JoyTerrainMaterial.ps";
	
	column_major_matrices	=	true;
	preprocessor_defines	=	function() return context.pixel_program_define; end;
};

local Program_PS_Deffered=
{
 	copy_from			= Program_PS_Default;
	source				= "JoyTerrainMaterial_DF.ps";
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

local Program_Ref_PS=
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
		
		if ( material_define.g_LightMapEnable == 1) then
			t["BeastLightRange"] = Ogre.Vector4( material_define.g_LightMapMinValue or 0.0 , material_define.g_LightMapMaxValue or 1.0 , 0.0 , 0.0);
		end
		
		if ( material_define.g_AlphaTestEnable == 1 ) then
			t["AlphaRef"] = material_define.g_AlphaRef/255;
		end;
		
		return t;
	end;
};

JoyTerrainMaterial={};

JoyTerrainMaterial.createMaterial = Material
{
	name = function() --这个时候context对象是不存在的
		_G.printf("JoyTerrainMaterial : %s\n",material_define.MaterialName);
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
				if ( material_define.g_LightMapEnable == 1 ) then
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
			
			--ploygon_mode=Ogre.Global.PM_WIREFRAME;
			
			TextureUnit
			{
				copy_from=TextureUnit_BottomLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_MiddleLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_UpperLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DetailLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_BlendLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_ColorMap;
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
				copy_from=Program_Ref_VS;
			};

			Program
			{
				copy_from=Program_PS_Default;
				program_name = get_program_name_ps( "default" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS;
			};
		};
	};
	
	Technique
	{
		doc = "default technique for common render";
		
		con	= function()
			
			if ( Hardware_SM3 == 0 or (g_Project == 0) ) then
				return false;
			else
				return true;
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
			
			--ploygon_mode=Ogre.Global.PM_WIREFRAME;
			
			TextureUnit
			{
				copy_from=TextureUnit_BottomLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_MiddleLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_UpperLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DetailLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_BlendLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_LightMap_Beast;
			};
			TextureUnit
			{
				copy_from=TextureUnit_ColorMap;
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
				copy_from=Program_Ref_VS;
			};

			Program
			{
				copy_from=Program_PS_Deffered;
				program_name = get_program_name_ps( "deffered" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS;
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
			
			--ploygon_mode=Ogre.Global.PM_WIREFRAME;
			
			TextureUnit
			{
				copy_from=TextureUnit_BottomLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_MiddleLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_UpperLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DetailLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_BlendLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_ColorMap;
			};
			TextureUnit
			{
				copy_from=TextureUnit_Complexity;
			};
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_SceneDepth;
				program_name = get_program_name_vs( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS;
			};

			Program
			{
				copy_from=Program_PS_SceneDepth;
				program_name = get_program_name_ps( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS;
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
				copy_from=Program_Ref_VS;
			};
			
			Program
			{
				copy_from=Program_PS_TexDensity;
				program_name = get_program_name_ps( "texDensity" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS;
			};
		};
	};
	
	Technique
	{
		doc = "default technique for pre render";
		
		con	= function()
			return material_define.g_LightMapEnable==1;
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
		--[[
		Pass
		{
				doc 		= 	"depth_pass";
				name		= 	"depth_pass";
				copy_from	=	BasePass;
				
				colour_write = false;
				depth_write = true;
				depth_check = true;
			
			--必须放在pass的最后，因为我们要根据texture_unit的信息构建program
			Program
			{
				copy_from=Program_VS_SceneDepth;
				program_name = get_program_name_vs( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_VS;
			};
			
			Program
			{
				copy_from=Program_PS_SceneDepth;
				program_name = get_program_name_ps( "sceneDepth" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS;
			};
			
		};
		--]]
		Pass
		{
			doc 		= 	"base_pass";
			name		= 	"base_pass";
			copy_from	=	BasePass;
			
			--depth_write = false;
			--depth_check = true;
			
			--ploygon_mode=Ogre.Global.PM_WIREFRAME;
			
			TextureUnit
			{
				copy_from=TextureUnit_BottomLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_MiddleLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_UpperLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_DetailLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_NormalLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_BlendLayer;
			};
			TextureUnit
			{
				copy_from=TextureUnit_LightMap_Beast;
			};
			TextureUnit
			{
				copy_from=TextureUnit_ColorMap;
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
				copy_from=Program_Ref_VS;
			};

			Program
			{
				copy_from=Program_PS_Pre;
				program_name = get_program_name_ps( "pre" );
			};

			Program_Ref
			{
				copy_from=Program_Ref_PS;
			};
		};
	};
	
};
