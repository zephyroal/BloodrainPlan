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

JoyMaterialCommon = {};

-- D3D��Texture Address Mode��Ogre ��Texture Address Mode��ת��
JoyMaterialCommon.AddressMode_DX2Ogre = function( mode )
	
	--[[ ��ʹ��D3D��׼
	if ( mode == 1 ) then
		v	=	Ogre.TextureUnitState.TAM_WRAP;
	elseif ( mode == 2 ) then
		v	=	Ogre.TextureUnitState.TAM_MIRROR;
	elseif ( mode == 3 ) then
		v	=	Ogre.TextureUnitState.TAM_CLAMP;
	elseif ( mode == 4 ) then
		v	=	Ogre.TextureUnitState.TAM_BORDER;
	else
		v	=	Ogre.TextureUnitState.TAM_WRAP;
	end
	return v;
	--]]
	return mode;
end

-- ��D3D��BlendModeת����OGRE��BlendMode��ȥ��
JoyMaterialCommon.BlendMode_DX2Ogre = function( mode )
	
	--[[ ��ʹ��D3D��׼
	if ( mode==1 ) then
		v	=	Ogre.Global.SBF_ZERO;
	elseif ( mode==2 ) then
		v	=	Ogre.Global.SBF_ONE;
	elseif ( mode==3 ) then
		v	=	Ogre.Global.SBF_SOURCE_COLOUR;
	elseif ( mode==4 ) then
		v	=	Ogre.Global.SBF_ONE_MINUS_SOURCE_COLOUR;
	elseif ( mode==5 ) then
		v	=	Ogre.Global.SBF_SOURCE_ALPHA;
	elseif ( mode==6 ) then
		v	=	Ogre.Global.SBF_ONE_MINUS_SOURCE_ALPHA;
	elseif ( mode==7 ) then
		v	=	Ogre.Global.SBF_DEST_ALPHA;
	elseif ( mode==8 ) then
		v	=	Ogre.Global.SBF_ONE_MINUS_DEST_ALPHA;
	elseif ( mode==9 ) then
		v	=	Ogre.Global.SBF_DEST_COLOUR;
	elseif ( mode==10 ) then
		v	=	Ogre.Global.SBF_ONE_MINUS_DEST_COLOUR;
	else
		v	=	Ogre.Global.SBF_ONE;
	end
	return v;
	--]]
	
	return mode;
	
end

-- ���� volume texture�Ĺ���ģ�ͣ���Ҫһ��texture unit�ṩ����ͼ
JoyMaterialCommon.CreateLightModelTextureUnit = function( p )
	local ts=p:createTextureUnitState();
	ts:setName("light_model");
	ts:setTextureName("light_model_A8L8.dds",Ogre.Global.TEX_TYPE_3D);
	ts:setTextureFiltering(Ogre.Global.FO_LINEAR,Ogre.Global.FO_LINEAR,Ogre.Global.FO_NONE);
	ts:setTextureAddressingMode(Ogre.TextureUnitState.TAM_CLAMP,Ogre.TextureUnitState.TAM_CLAMP,Ogre.TextureUnitState.TAM_CLAMP);
end

JoyMaterialCommon.OpenOrCreateMaterial = function( name )
	local mm=Ogre.MaterialManager.getSingletonPtr();
	local mat=mm:createOrRetrieve(name,"General")
	mat:removeAllTechniques();
	mat:createTechnique():createPass();
	return mat;
end

-- Utility Funcs

function any_one(a,b)
	if a==1 or b==1 then
		return 1
	else
		return 0
	end;
end;

function all_one(a,b)
	if a==1 and b==1 then
		return 1
	else
		return 0
	end;
end;

function option_call(f,default_value,...)
	if f==nil then
		return default_value;
	end
	if type(f)=="function" then
		return f(...)
	end
	return f;
end

function context_value(value_name)
	return function()
		return material_define[value_name];
	end;
end

function context_bool(value_name,v)
	return function()
		return material_define[value_name]==v;
	end;
end

function context_greater(value_name,v)
	return function()
		if material_define[value_name] == nil then
			return false;
		end;
		return material_define[value_name] > v;
	end;
end

--���material_define[value_name]��1,�� ����true_value�����򷵻� false_value
function context_iif(value_name,true_value,false_value)
	return function()
		if(material_define[value_name]==1) then
			return true_value;
		else
			return false_value;
		end;
	end;
end

--���material_define[value_name]��1,�� ���ش���� true_value_name ������������ֵ�����򷵻� false_value_name
function context_iif_value(value_name,true_value_name,false_value_name)
	return function()
		if(material_define[value_name]==1) then
			return material_define[true_value_name];
		else
			return material_define[false_value_name];
		end;
	end;
end

function space(count)
	return string.rep('  ',count);
end

--��ʽ  copy_from=basetable
function copy_table_from_base(_def)
	
	local def;
	if(type(_def)=="function")then
		def=_def();
	else
		def=_def;
	end;

	if(def.copy_from~=nil)then
		local base=def.copy_from;
		if(type(base)=="function")then
			base=base();
		else
			base=base;
		end;
		
		--�����༶�̳�
		copy_table_from_base(base);
		
		local t={};
		table.foreach(base,function(k,v)
			t[k]=v;
			end);
		table.foreach(def,function(k,v)
			t[k]=v;
			end);
		table.foreach(t,function(k,v)
			def[k]=v;
			end);
	end;
end;

function parse_boolean(v)
	if(v==nil)then
		return false;
	end
	if(type(v)=="boolean")then
		return v;
	end
	if(type(v)=="number")then
		if(v<0.00001 or v>-0.00001)then
			return false;
		else
			return true;
		end
	end
	if(type(v)=="string") then
		if(v=="on" or v=="yes" or v=="true" or v=="1")then
			return true;
		elseif(v=="off" or v=="no" or v=="false" or v=="0")then
			return false;
		else
			error("No the bool flag! [" .. v .. "]");
		end;
	end;
	error("No the bool flag! [" .. tostring(v) .. "]");
end

parse_bool=parse_boolean;

--��� non_enable_field �ֶε�ֵΪ0,�򷵻� texture_field ָ���� �Ƕ�����ͼ��һ���ַ����� ,
getUnAnimationTexture=function(non_enable_field,texture_field)
	return function()
		if( non_enable_field==nil or non_enable_field=="" or material_define[non_enable_field]~=1  )then
			if ( non_enable_field ~= "ForceEnable" ) then
				return nil;
			else
				return material_define[texture_field];
			end;
		end;
		return material_define[texture_field];
	end;
end;

--��� enable_field ָ����ֵΪ�棬�򷵻� texture_field ָ���ı�����ֵ����ѱ���������ֵ, һ���� talbe,�洢��ͼ�������б���һ���� ���¼�� )
getAnimationTexture=function(enable_field,texture_field,time_field)
	return function()
		--�ϵĸ�ʽ:
		if( enable_field==nil or enable_field=="" or material_define[enable_field]==1 )then
			local v=material_define[texture_field];
			if(is_table(v))then
				return v,(material_define[time_field] or 1);
			else
				printf("error:animation texture material_define error materail is [%s] (2)",material_define.MaterialName);
			end;
		else
			if(enable_field=="g_Diffuse1Type" and material_define["g_DiffuseType"]==1 )then
				return material_define["T_AD"],material_define["g_Duration"];
			end;
		end;
		return nil;
	end;
end;

--����ָ�����ֶ����ƻ�ȡ texture_address_mode;������������ֵ
getTextureAddressMode=function(mode_field_1,mode_field_2)
	return function()
		return JoyMaterialCommon.AddressMode_DX2Ogre(material_define[mode_field_1]),JoyMaterialCommon.AddressMode_DX2Ogre(material_define[mode_field_2]),Ogre.TextureUnitState.TAM_WRAP;
	end;
end;

--��ȡ������ͼ�Ĺ���ģʽ
getNormalFilter=function()
	return function()
		return Ogre.Global.FO_ANISOTROPIC,Ogre.Global.FO_ANISOTROPIC,Ogre.Global.FO_LINEAR ;
	end;
end;

--��ȡDiffuse��ͼ�Ĺ���ģʽ
getDiffuseFilter=function()
	return function()
		if(material_define.g_AlphaTestEnable==1)then
			return Ogre.Global.FO_LINEAR,Ogre.Global.FO_LINEAR,Ogre.Global.FO_POINT;
		else
			return Ogre.Global.FO_LINEAR,Ogre.Global.FO_LINEAR,Ogre.Global.FO_POINT;
		end;
	end;
end;

getPointFilter=function()
	return function()
		return Ogre.Global.FO_POINT,Ogre.Global.FO_POINT,Ogre.Global.FO_POINT;
	end;
end;

--��ȡCube Map�Ĺ���ģʽ
getCubeMapFilter=function()
	return function()
		return Ogre.Global.FO_LINEAR,Ogre.Global.FO_LINEAR,Ogre.Global.FO_POINT;
	end;
end;

--��ȡUV����
getScrollAnim=function(enable_field,u_field,v_field,fixed_field)
	return function()
		if(enable_field==nil or enable_field=="" or material_define[enable_field]~=0)then
			return material_define[u_field],material_define[v_field],material_define[fixed_field];
		else
			return nil;
		end;
	end;
end;

getRotateAnim=function(enable_field,r_field,fixed_field)
	return function()
		if(enable_field==nil or enable_field=="" or material_define[enable_field]~=0)then
			return material_define[r_field],material_define[fixed_field];
		else
			return nil;
		end;
	end;
end;

getScaleAnim = getScrollAnim;

BaseTextureUnit=
{
	con = function()
		return true;
	end;
	filtering = function() return "trilinear"; end;
};

--����ģ�͵���ͼģ��
TextureUnit_LightModel=
{
	copy_from	=	BaseTextureUnit;
	name		=	"light_model";
	con			=	true;
	texture		=	"light_model_A8L8.dds";
	type		=	Ogre.Global.TEX_TYPE_3D;
	sampler_register_name	="SLM";
	filtering				=function()return Ogre.Global.FO_LINEAR,Ogre.Global.FO_LINEAR,Ogre.Global.FO_NONE;end;
	tex_address_mode		="clamp";
};

--����ͼ TextureUnitģ��
LightMapTextureUnit=
{
	copy_from	=	BaseTextureUnit;
	name		=	"light_map";
	con			=	context_bool("g_LightMapEnable",1);
	texture		=	getUnAnimationTexture(nil,"T_LM");
	type		=	Ogre.Global.TEX_TYPE_2D;
	sampler_register_name	=	"SLMT";
	filtering				=	function()return Ogre.Global.FO_LINEAR,Ogre.Global.FO_LINEAR,Ogre.Global.FO_NONE;end;
	tex_address_mode		=	"clamp";
};

BasePass=
{
	name = "base_pass";

	after_create = function()
		context.samplers={};
	end;

	scene_blend = function()
		if ( material_define.g_AlphaBlendEnable == 1 ) then
			return JoyMaterialCommon.BlendMode_DX2Ogre(material_define.g_SrcBlend),JoyMaterialCommon.BlendMode_DX2Ogre(material_define.g_DestBlend);
		else
			return nil;
		end;
	end;

	depth_check		=	context_bool("g_ZEnable",1);
	depth_write		=	context_bool("g_ZWriteEnable",1);
	colour_write	=	context_bool("g_ColorWriteEnable",1);
	
	depth_func = function()
		return Ogre.Global.CMPF_LESS_EQUAL; 
	end;

	--bias���������ƵĹ�ʽΪ��Bias = DepthBias * r + slopeScaleDepthBias * maxDepthSlope;
	--����rΪdepth-buffer�е���Сֵ
	depth_bias = function()
		if ( material_define.g_DepthBiasEnable==1 ) then
			return 0.5,0.5;
		end;
		return nil;
	end;

	cull_hardware = function()
		if ( material_define.g_CullMode == 2 ) then
			return nil;
		elseif ( material_define.g_CullMode == 3 ) then
			return Ogre.Global.CULL_ANTICLOCKWISE;
		else
			return Ogre.Global.CULL_NONE;
		end
	end;

	cull_software = function()
		if ( material_define.g_CullMode == 2 ) then
			return nil;
		elseif ( material_define.g_CullMode == 3 ) then
			return Ogre.Global.MANUAL_CULL_FRONT;
		else
			return Ogre.Global.MANUAL_CULL_NONE;
		end
	end;
	
	--[[
	alpha_rejection = function()
		if ( material_define.g_AlphaTestEnable==1 ) then
			return Ogre.Global.CMPF_GREATER_EQUAL,material_define.g_AlphaRef;
		end;
	end;
	--]]
	
	alpha_rejection = nil;
	alpha_to_coverage = function()
		return false;
	end;
	
	diffuse = function()
	  if ( material_define.g_Diffuse ) then
	     if ( material_define.g_DiffuseAlpha ) then
	        material_define.g_Diffuse.a=material_define.g_DiffuseAlpha;
	     end;
	     return material_define.g_Diffuse;
	  else
	     return Ogre.ColourValue(1.0,1.0,1.0,1.00);
	  end;
	end;

	ambient = function()
	  if ( material_define.g_Ambient ) then
	     return material_define.g_Ambient;
	  else
	     return Ogre.ColourValue(1.0,1.0,1.0,1.0);
	  end;
	end;
	
	specular = function()
		if ( material_define.g_Specular ) then
			return material_define.g_Specular;
		else
			return Ogre.ColourValue(1,1,1,1);
		end;
	end;
	
	emissive = function()
		if ( material_define.g_Emissive ) then
			return material_define.g_Emissive;
		else
			return Ogre.ColourValue(0,0,0,0);
		end;
	end;
	
	shininess = function()
		if ( material_define.g_Shininess ) then
			return material_define.g_Shininess;
		else
			return 0.0;
		end;
	end;
	
	polygon_mode = function()
	  if ( material_define.g_PolygonMode == nil ) then
	    return Ogre.Global.PM_SOLID;
	  end;
	  if ( material_define.g_PolygonMode == 3 ) then
	    return Ogre.Global.PM_SOLID;
	  end;
	  if ( material_define.g_PolygonMode == 2 )then
	    return Ogre.Global.PM_WIREFRAME;
	  end;
	  if ( material_define.g_PolygonMode == 1 )then
	    return Ogre.Global.PM_POINTS;
	  end;
	end;
};