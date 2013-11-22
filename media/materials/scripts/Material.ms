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
 
-- Lua端记录材质及编译shader所用的时间
material_time=
{
	name			= "创建材质";
 	total_time		= 0.0;
 	total_count		= 0;
 	max_time		= 0.0;
 	max_time_name	= "";
}
 
compile_program_time=
{
 	name			= "重新编译";
 	total_time		= 0.0;
 	total_count		= 0;
 	max_time		= 0.0;
 	max_time_name	= "";
}

load_program_time=
{
 	name			= "缓存载入";
 	total_time		= 0.0;
 	total_count		= 0;
 	max_time		= 0.0;
 	max_time_name	= "";
}

cache_program_time=
{
 	name			= "创建缓存";
 	total_time		= 0.0;
 	total_count		= 0;
 	max_time		= 0.0;
 	max_time_name	= "";
}

-- 显示材质编译时间
show_material_time = function()

	local m = 
	{
		compile_program_time;
		load_program_time;
		cache_program_time;
		material_time;
	};
	
	for i,t in ipairs(m) do
		local avg = 0;
		if ( t.total_count > 0 ) then
			avg  = t.total_time / t.total_count;
		end;
		
		_G.printf( "%s 时间：count=%.2f,total_time=%0.2f,avg=%0.2f,max_time=%0.2f,max is [%s]\n",
			t.name,
			t.total_count,
			t.total_time,
			avg,
			t.max_time,
			t.max_time_name
		);
	end;

end;

-- 添加时间
local add_time = function( t, new_time, program_name )

	if ( new_time > t.max_time ) then
		t.max_time 		= new_time;
		t.max_time_name = program_name;
	end;
	
	t.total_time 	= t.total_time + new_time;
	t.total_count	= t.total_count + 1;
	
	_G.printf( "%s 时间：count=%.2f,total_time=%0.2f,avg=%0.2f,max_time=%0.2f,max is [%s]\n",
		t.name,
		t.total_count,
		t.total_time,
		t.total_time/t.total_count,
		t.max_time,
		t.max_time_name
	);

end;

OgreMaterial = {};

-- todo : 添加Cache策略，添加CacheGroup qtx
OgreMaterial.CacheGroup = function( def )
end;

--[[
	material_define 中可以指定只创建某个Technique.这种情况下，其他已经存在的Tec不被修改。只有指定名字的Tec被重新生成。
	format: CreateTechnique=string or nil;
	CreateTechnique="default"; 	--只生成default技术
	CreateTechnique=nil			--不生成技术
	CreateTechnique="all"		--生成所有技术
--]]

OgreMaterial.Material = function( def )

	_G.printf( "template:material" );
	
	local ret = option_call( def.mode, nil );
	
	copy_table_from_base( def );
	
	return function( material_define )
	
		local start_time_create_material = getTime_s();
		if ( type(material_define) ~= "table" ) then
			error( string.format("material_define 必须是一个表!实际传入的是%s,数据=%s",type(material_define),tostring(material_define)) );
		end;
		
		local e 			= getfenv(1);
		e.context			= {};
		e.material_define	= material_define;
		
		if ( material_define.CreateTechnique == "" ) then
			material_define.CreateTechnique = nil;
		end;
		
		if ( material_define.CreateTechnique == nil ) then
			material_define.CreateTechnique = "all";
		end;
		
		if ( material_define.CreateTechnique ~= nil ) then
			if ( type(material_define.CreateTechnique) ~= "string" ) then
				error( "material_define.CreateTechnique must be string!!!" );
				return;
			end;
		end;
		
		local oc = function( f,d )
			return option_call( f,d );
		end;
		
		local material_name = oc( def.name, nil );
		if ( material_name == nil ) then
			error( "no material name" );
		end;
		
		_G.printf( "start process material: [%s]",material_name);
		
		if ( oc(def.con,true) == false ) then
			_G.printf( "condition false" );
			return;
		end;
		
		if ( EffectLoaderImpl~=nil and EffectLoaderImpl.loadingStage~=nil ) then
			EffectLoaderImpl.loadingStage( "ExternalStages", material_name );
		end
		
		mm = Ogre.MaterialManager.getSingletonPtr();
		
		-- 对于已经存在的材质我们是否需要重新编译
		if ( mm:resourceExists( material_name ) ) then
			if ( material_define.CreateTechnique == nil ) then
				return;
			end;
			
			local tmp_mat = mm:createOrRetrieve( material_name, "General" );
			-- 如果已经包含了当前需要创建的Tec，直接返回
			if ( tmp_mat:getTechnique( material_define.CreateTechnique ) ) then
				return;
			end;
		end;
		
		-- 创建替换材质
		createReplaceErrorMaterial = function( scheme_name )
			_G.printf( "没有支持的技术" );
			_G.printf( "  原因:".. mat:getUnsupportedTechniquesExplanation() );
			mat:removeAllTechniques();
			local tec=mat:createTechnique();
			tec:setName(scheme_name);
			tec:setSchemeName(scheme_name);
			local pass=tec:createPass();
			local texture=pass:createTextureUnitState();
			texture:setTextureName( "error.dds",Ogre.Global.TEX_TYPE_2D );
			_G.printf( "创建替换材质完成" );
		end;
		
		local exists = mm:resourceExists( material_name );
		mat = mm:createOrRetrieve( material_name, "General" );
		if ( mat.__ok == false ) then
			error( "Create Material Failed![%s]", material_name );
		end;
		
		if ( material_define.CreateTechnique == "all" ) then
			mat:removeAllTechniques();
		end;
		
		context.material		=	mat;
		context.technique		=	nil;
		context.pass			=	nil;
		context.texture_unit	=	nil;
		context.vertex_program	=	nil;
		context.pixel_program	=	nil;
		
		oc( def.after_create, nil );
		
		table.foreachi( def, function(i,v)
			v();
		end)
		
		_G.printf( "prepare to compile material\n" );
		
		-- 判断材质是否有被支持的技术
		if ( mat:getNumTechniques() < 1 ) then
			_G.printf( "Material Has Not any technique.[%s]", mat:getName() );
			createReplaceErrorMaterial( material_define.CreateTechnique or "notechnique" );
		end;
		
		local ret = mat:compile();
		
		if ( ret == false ) then
			_G.printf( "材质编译错误[%s]！！", material_name );
		else
			_G.printf( "compile material finished\n" );
			if ( mat:getNumSupportedTechniques() < 1 ) then
				createReplaceErrorMaterial(material_define.CreateTechnique or "nosupporttechnique");
			end;

			local export_file = oc( def.export_file );
			if ( export_file ) then
				_G.printf( "prepare material export\n" );
				ms=Ogre.MaterialSerializer();
				local exportMaterialFileName;
				if ( is_string(export_file) ) then
					exportMaterialFileName=export_file;
				else
					exportMaterialFileName="e:\\" .. string.gsub(material_name,"/","_") .. ".txt";
				end;
				_G.printf( "Exporting material to [%s]", exportMaterialFileName );
				ms:exportMaterial( mat, exportMaterialFileName, false, true, "", "" );
				_G.printf("export ok!");
			end;

		end;
		
		local end_time_createMaterial = getTime_s();
		add_time( material_time, end_time_createMaterial-start_time_create_material, material_name );
	
	end;

end;

OgreMaterial.Technique = function( def )

	_G.printf("%stemplate: technique",space(1));
	
	local ret = option_call( def.mode, nil );
	
	copy_table_from_base(def);
	
	return function()
	
		local oc = function(f,d)
			return option_call(f,d);
		end
		
		if ( oc(def.con,true) == false ) then
			return;
		end
		
		local name = oc(def.name,"default");
		if ( material_define.CreateTechnique ~= "all" and material_define.CreateTechnique ~= name ) then
			--不是当前想创建的Tec,直接返回
			return;
		end;
		
		local tec = nil;
		tec = oc(def.create,nil);
		
		if ( tec == nil ) then
			if ( context.material:getTechnique(name) ) then
				tec	= context.material:getTechnique(name);
				tec:removeAllPasses();
			else
				tec=context.material:createTechnique();
			end;
		end
		
		if ( name ) then
		  tec:setName(name);
		end;
		
		if ( tec.__ok == false ) then
			error( "Create Technique Failed in material [%s]",context.material:getName() );
		end;
		
		context.technique = tec;
		oc(def.after_create,nil);
		
		local scheme = oc(def.scheme,"default");
		tec:setSchemeName(scheme);
		
		--shadow_caster_material
		--string
		local shadow_caster_material = oc( def.shadow_caster_material, nil );
		if ( shadow_caster_material ) then
			tec:setShadowCasterMaterial( shadow_caster_material );
		end;
		
		--shadow_receiver_material
		--string
		local shadow_receiver_material = oc( def.shadow_receiver_material, nil );
		if ( shadow_receiver_material ) then
			tec:setShadowReceiverMaterial( shadow_receiver_material );
		end;
		
		_G.printf( "%sTechnique [%s]", space(1), tec:getSchemeName() );
		
		table.foreachi( def, function(i,v)
			v();
		end)
	
	end;

end;

OgreMaterial.Pass = function( def )

	_G.printf("%stemplate: pass",space(2));
	
	local ret = option_call( def.mode, nil );
	
	copy_table_from_base(def);
	
	return function()
	
		local start_time_pass;
		local end_time_pass;
		start_time_pass=getTime_s();
		
		_G.printf( "create pass\n" );
		
		local oc = function(f,d)
			return option_call(f,d);
		end
		if ( oc(def.con,true) == false ) then
			return;
		end
		
		local pass = oc( def.create, nil );
		if ( pass == nil ) then
			pass=context.technique:createPass();
		end
		
		if ( pass == nil ) then
			error("Create Pass Failed in material [%s]",context.material:getName());
		end;
		
		context.pass = pass;
		oc(def.after_create,nil);
		
		local name = oc(def.name,"base");
		pass:setName(name);
		
		local function parse_simple_blend( name )
			local s0,s1;
			if ( name == Ogre.Global.SBT_ADD or name == "add" ) then
				s0	=	Ogre.Global.SBF_ONE;
				s1	=	Ogre.Global.SBF_ONE;
			elseif ( name == Ogre.Global.SBT_MODULATE or name == "modulate" ) then
				s0	=	Ogre.Global.SBF_DEST_COLOUR;
				s1	=	Ogre.Global.SBF_ZERO;
			elseif ( name == Ogre.Global.SBT_TRANSPARENT_COLOUR or name == "colour_blend" ) then
				s0	=	Ogre.Global.SBF_SOURCE_COLOUR;
				s1	=	Ogre.Global.SBF_ONE_MINUS_SOURCE_COLOUR;
			elseif ( name == Ogre.Global.SBT_TRANSPARENT_ALPHA or name=="alpha_blend" ) then
				s0	=	Ogre.Global.SBF_SOURCE_ALPHA;
				s1	=	Ogre.Global.SBF_ONE_MINUS_SOURCE_ALPHA;
			elseif ( name == Ogre.Global.SBT_REPLACE or name == "replace" ) then
				s0	=	Ogre.Global.SBF_ONE;
				s1	=	Ogre.Global.SBF_ZERO;
			else
				error("simple blend param error ["..tostring(name).."]");
			end;
			return s0,s1;
		end;
		
		--scene_blend 格式：
		--1：string;
		--2：int
		--3：int,int;
		local s1,s2 = oc( def.scene_blend, nil );
		if ( s1 ) then
			local c1;
			local c2;
			if( is_string(s1) ) then
				c1,c2=parse_simple_blend(s1);
			elseif is_number(s1) then
				if is_number(s2) then
					c1=s1;
					c2=s2;
				else
					c1,c2=parse_simple_blend(s1);
				end;
			else
				error("scene_blend need [s] or [i] or [i,i]");
			end;
			pass:setSceneBlending(c1,c2);
		end;
		
		--separate_scene_blend 格式：
		--1：string,string;
		--2：int,int
		--3：int,int,int,int;
		local s1,s2,s3,s4 =oc(def.separate_scene_blend ,nil);
		if(s1)then
			local c1;
			local c2;
			local a1;
			local a2;

			if(s3==nil )then
				if( all_is_string(s1,s2) or all_is_number(s1,s2) )then
					c1,c2=parse_simple_blend(s1);
					a1,a2=parse_simple_blend(s2);
				else
					error("separate_scene_blend need1 [s,s] or [i,i] or [i,i,i,i]");
				end;
			elseif(all_is_number(s1,s2,s3,s4)) then
				c1=s1;
				c2=s2;
				a1=s3;
				a2=s4;
			else
				error("separate_scene_blend need2 [s,s] or [i,i] or [i,i,i,i]");
			end;
			pass:setSeparateSceneBlending(c1,c2,a1,a2);
		end;
		
		--depth_check 格式:
		--1: bool;
		--2: int;
		pass:setDepthCheckEnabled( oc(def.depth_check,true) );
		pass:setDepthWriteEnabled( oc(def.depth_write,true) );
		pass:setColourWriteEnabled( oc(def.colour_write,true) );
		
		-- depth_func
		local func = oc(def.depth_func,Ogre.Global.CMPF_LESS_EQUAL);
		if ( func ) then
			pass:setDepthFunction( func );
		end;
		
		--depth_bias 格式:
		--float,<float>
		local s1,s2=oc(def.depth_bias,nil)
		if(s1)then
			if(s2==nil and is_number(s1) )then
				--pass:setDepthBias( s1,0 );
			elseif( all_is_number(s1,s2) ) then
				--pass:setDepthBias( s1,s2 );
			else
				error("depth_bias need [f,<f>]");
			end;
		end;
		
		-- iteration_depth_bias
		-- todo qtx
		
		--alpha_rejection 格式:
		--int,<int>
		
		local s1,s2=oc(def.alpha_rejection,nil)
		if(s1)then
			if(is_number(s1))then
				pass:setAlphaRejectFunction(s1);
			else
				error("alpha_rejection function need int");
			end;

			if(is_number(s2))then
				pass:setAlphaRejectValue(s2);
			else
				if(not(s1==Ogre.Global.CMPF_ALWAYS_FAIL or s1==Ogre.Global.CMPF_ALWAYS_PASS))then
					error("alpha_rejection value need int!");
				end;
			end;
		end;

		
		--alpha_to_coverage 格式:
		--bool
		local s1=oc(def.alpha_to_coverage,false);
		pass:setAlphaToCoverageEnabled(s1);
		
		--normalise_normals
		--bool
		local s1=oc(def.normalise_normals,false);
		pass:setNormaliseNormals(s1);
		
		--transparent_sorting
		--bool
		local s1=oc(def.transparent_sorting,nil);
		if(s1)then
			pass:setTransparentSortingEnabled(s1);
		end;
		
		--cull_hardware
		--int
		--string
		local s1=oc(def.cull_hardware,nil);
		if(s1)then
			if is_number(s1) then
				pass:setCullingMode(s1);
			elseif is_string(s1) then
				local t=
				{
					["clockwise"]=Ogre.Global.CULL_CLOCKWISE;
					["anticlockwise"]=Ogre.Global.CULL_ANTICLOCKWISE;
					["none"]=Ogre.Global.CULL_NONE;
				};
				if(t[s1])then
					pass:setCullingMode(t[s1]);
				else
					error("cull_hardware error string:["..s1.."]");
				end;
			else
				error("cull_hardware need int or string");
			end;
		end;
		
		--cull_software
		--int
		--string
		local s1=oc(def.cull_software,nil);
		if(s1)then
			if is_number(s1) then
				pass:setManualCullingMode(s1);
			elseif is_string(s1) then
				local t=
				{
					["back"]=Ogre.Global.MANUAL_CULL_BACK;
					["front"]=Ogre.Global.MANUAL_CULL_FRONT;
					["none"]=Ogre.Global.MANUAL_CULL_NONE;
				};
				if(t[s1])then
					pass:setManualCullingMode(t[s1]);
				else
					error("cull_software error string:["..s1.."]");
				end;
			else
				error("cull_software need int or string",2);
			end;
		end;
		
		--point_sprites
		--point_sprites <"true"|"false">
		local s1=oc(def.point_sprites,nil);
		if(s1)then
			sa=parse_boolean(s1);
			pass:setPointSpritesEnabled(s1);
		end;

		--point_size
		--point_size size_in_pixel
		local s1=oc(def.point_size,nil);
		if(s1)then
			if(is_number(s1))then
				pass:setPointSize(s1);
			else
				error("point_size need number",2);
			end;
		end;
		
		--point_size_min <size> Default: point_size_min 0
		local s1=oc(def.point_size_min,nil);
		if(s1)then
			if(is_number(s1))then
				pass:setPointMinSize(s1);
			else
				error("point_size_min need number",2);
			end;
		end;
		
		--point_size_max <size> Default: point_size_max 0
		local s1=oc(def.point_size_max,nil);
		if(s1)then
			if(is_number(s1))then
				pass:setPointMaxSize(s1);
			else
				error("point_size_min need number",2);
			end;
		end;
		
		--point_size_attenuation
		--point_size_attenuation <on|off> [constant linear quadratic]
		--Default: point_size_attenuation off
		local s1,s2,s3,s4=oc(def.point_size_attenuation,nil);
		if(s1)then
			if(is_table(s1))then
				local s=s1;
				s1=s[1];
				s2=s[2];
				s3=s[3];
				s4=s[4];
			end;

			local v=parse_bool(s1);
			s2=s2 or 0;
			s3=s3 or 0;
			s4=s4 or 0;
			if(is_number(s2) and is_number(s3) and is_number(s4))then
				pass:setPointAttenuation(v,s2,s3,s4);
				_G.print("set setPointAttenuation =".. tostring(v) );
			else
				error("point_size_attenuation format error!need 1 bool,3 number",2);
			end;
		end;


		--ambient
		local s1 = oc( def.ambient, nil );
		printf( type(s1) );
		if ( s1 ) then
			pass:setAmbient(s1);
		end;
		
		--diffuse
		local s1=oc(def.diffuse,nil);
		printf( type(s1) );
		if(s1)then
			pass:setDiffuse(s1);
		end;
		
		--specular
		local s1 = oc( def.specular, nil );
		printf( type(s1) );
		if ( s1 ) then
			pass:setSpecular(s1);
		end;
		
		--emissive
		local s1 = oc( def.emissive, nil );
		printf( type(s1) );
		if ( s1 ) then
			pass:setSelfIllumination(s1);
		end;
		
		--shininess
		local s1 = oc( def.shininess, nil );
		printf( type(s1) );
		if ( s1 ) then
			pass:setShininess(s1);
		end;
		
		--polygon_mode
		--polygon_mode <"solid"|"wireframe"|"points">
		--polygon_mode int
		local s1=oc(def.polygon_mode,nil);
		if(s1)then
			if(is_string(s1)) then
				if(s1=="solid")then
					s1=Ogre.Global.PM_SOLID;
				elseif(s1=="wireframe")then
					s1=Ogre.Global.PM_WIREFRAME;
				elseif(s1=="points") then
					s1=Ogre.Global.PM_POINTS;
				else
					error( string.format("polygon_mode error(string),[%s] material is[%s]",s1,context.material:getName()) );
				end;
			end;
			if is_number(s1) then
				if(s1==Ogre.Global.PM_POINTS or s1==Ogre.Global.PM_SOLID or s1==Ogre.Global.PM_WIREFRAME)then
					pass:setPolygonMode(s1);
				else
					error( string.format("polygon_mode error, material is[%s]",context.material:getName()) );
				end;
			else
				error( string.format("polygon_mode error,[%s],need string or int,material is[%s]",tostring(s1),context.material:getName()) );
			end;
		end;
		
		-- 1.64版本不支持，添加 qtx
		if(pass:isTransparent())then
			--pass:setTransparentSortingForced(true);
		end;
		
		_G.printf("%sPass [%s]",space(2),pass:getName());
		
		table.foreachi(def,function(i,v)
			if(is_function(v))then
				v();
			else
				error("Pass process child %d,error,get non function; v=[%s]",i,tostring(v) );
			end;
		end)
		
		end_time_pass=getTime_s();
	
	end;

end;

OgreMaterial.parse_program_type = function ( program_type )

	if ( program_type == nil ) then
		error("Program need program_type [" .. tostring(program_type) .."]");
		return;
	else
		if ( is_string(program_type) ) then
			if ( program_type == "vertex" or program_type == "vs" ) then
				program_type = Ogre.Global.GPT_VERTEX_PROGRAM;
			elseif ( program_type == "pixel" or program_type == "ps" or program_type == "fragment" ) then
				program_type = Ogre.Global.GPT_FRAGMENT_PROGRAM;
			else
				error("Program don't support the type ["..tostring(program_type).."]");
				return;
			end;
		else
			if ( program_type == Ogre.Global.GPT_VERTEX_PROGRAM or
				 program_type == Ogre.Global.GPT_FRAGMENT_PROGRAM
			)then

			else
				error("Program don't support the type ["..tostring(program_type).."]");
				return;
			end;
		end;
	end;
	
	return program_type;
	
end;

--[[ qtx
named_auto_constants=
{
	{"worldviewproj_matrix"},			--根据参数名称自动推导参数语义， 额外参数缺省为0
	{"worldviewproj_matrix","auto",1},	--根据参数名称自动推导参数语义
	{"worldviewproj_matrix",Ogre.Global.ACT_WORLDVIEWPROJ_MATRIX},
	{"worldviewproj_matrix",Ogre.Global.ACT_WORLDVIEWPROJ_MATRIX,0},
};
named_auto_constants_real=
{
	{"time",Ogre.Global.ACT_TIME,1.0},
	{"time"},
};
--]]

OgreMaterial.process_named_auto_constants = function(gpp,params,is_real)
	if ( params ) then
		for _,param in ipairs(params) do
			n,s,p=unpack(param);
			if(not is_string(n))then
				error("program params name must is string.");
			end;
			if(s==nil or s=="" or s=="auto")then
				s="ACT_".. string.upper(n);
				s=Ogre.Global[s];
				if(s==nil)then
					error("program params semantics not exists name=["..n.."]");
				end;
			end;
			if(is_string(s))then
				s="ACT_".. string.upper(s);
				s=Ogre.Global[s];
				if(s==nil)then
					error("program params semantics not exists name=["..n.."]");
				end;
			end;
			if( not is_number(s))then
				error("program params semantics need int name=["..n.."] semantics=["..tostring(s).."]" );
			end;

			if(p==nil)then
				p=0;
				if(s==Ogre.Global.ACT_TIME)then
					p=1.0;
				end;
			end;

			if(is_real==true)then
				gpp:setNamedAutoConstantReal(n,s,p);
			else
				gpp:setNamedAutoConstant(n,s,p);
			end;
		end;
	end;
end;

--[[ qtx
named_constants=
{
	g_vlaue1=Ogre.Vector4(1,1,2,3);
	g_vlaue2=Ogre.Vector4(1,1,2,3);
};
--]]

OgreMaterial.process_named_constants=function(gpp,params)
	if(params)then
		for k,param in pairs(params) do
			if(not is_string(k))then
				error("named constants need key is string!");
			end;
			gpp:setNamedConstant(k,param);
		end;
	end;
end;

OgreMaterial.Program = function(def)
	_G.printf("%stemplate: program ",space(3));
	
	local ret = option_call( def.mode, nil );
	
	copy_table_from_base(def);
	
	return function()
	
		_G.printf("%sprogram process",space(3));
		local oc=function(f,d)
			return option_call(f,d);
		end;
		if( oc( def.con, true ) == false ) then
			return;
		end;

		local program_name = oc(def.program_name,nil);
		if(program_name==nil)then
			error("Program need [name]!!");
			return;
		end;

		local start_time=getTime_s();

		local program_type = oc(def.program_type,nil);

		program_type = OgreMaterial.parse_program_type( program_type );

		local language = oc(def.language,nil);
		if ( language == nil ) then
			error("Program need [language]!");
			return;
		end;

		local cache_group = oc(def.cache_group,nil);
		if ( shader_cache_enable == true ) then
			if ( cache_group == nil ) then
				error("Program need [cache_group]!!");
				return;
			end;
		end;

		--local program=
		local gm=Ogre.GpuProgramManager.getSingletonPtr();

		--如果该program已经被创建了，我们直接使用这个,不需要再进行后续的处理
		local gp = nil;
		gp = gm:getByName(program_name);
		if ( gp ~= nil ) then
			if isEditor==1 then
				----
			else
				context.program=gp;
				if ( program_type == Ogre.Global.GPT_VERTEX_PROGRAM ) then
					context.vertex_program=gp;
				elseif ( program_type == Ogre.Global.GPT_FRAGMENT_PROGRAM ) then
					context.pixel_program=gp;
				end;
				return;
			end;
		else
		gp = nil;
		end;

		local is_create_from_cached=true;

		if gp == nil then
			is_create_from_cached = false;
			gp = nil;
			local gm = Ogre.HighLevelGpuProgramManager.getSingletonPtr();
			gp = gm:createProgram( program_name, "General", language, program_type );

			if ( gp == nil or gp.__ok == false ) then
				error(string.format("Create GPUProgram [%s] Failed in material [%s]",program_name,material_define.MaterialName));
			end;
		else
			is_create_from_cached=true;
		end
		
		context.program = gp;
		if ( program_type == Ogre.Global.GPT_VERTEX_PROGRAM ) then
			context.vertex_program = gp;
		elseif ( program_type == Ogre.Global.GPT_FRAGMENT_PROGRAM ) then
			context.pixel_program = gp;
		end;

		if ( is_create_from_cached ~= true ) then
			local source = oc(def.source,nil);
			if ( source ) then
				gp:setSourceFile(source);
			else
				error("Program need source!");
			end;

			local entry_point = oc(def.entry_point,nil);
			if ( entry_point ) then
				gp:setParameter("entry_point",entry_point);
			else
				error("Program need entry_point!");
			end;

			local target = oc(def.target,nil);
			if ( target == nil ) then
				target=oc(def.profiles,nil);
			end;
			if(target) then
				gp:setParameter("target",target);
			else
				error("Program need target or profiles!");
			end;


			local includes_skeletal_animation=oc(def.includes_skeletal_animation,false);
			gp:setSkeletalAnimationIncluded(includes_skeletal_animation);

			local includes_morph_animation=oc(def.includes_morph_animation,false);
			gp:setMorphAnimationIncluded(includes_morph_animation);

			local includes_pose_animation=oc(def.includes_pose_animation,0);
			gp:setPoseAnimationIncluded(includes_pose_animation);

			local uses_vertex_texture_fetch=oc(def.uses_vertex_texture_fetch,false);
			--gp:setVertexTextureFetchRequired(uses_vertex_texture_fetch);

			local uses_adjacency_information=oc(def.uses_adjacency_information,false);
			--gp:setAdjacencyInfoRequired(uses_adjacency_information);

			local column_major_matrices=oc(def.column_major_matrices,false);
			gp:setParameter("column_major_matrices",tostring(column_major_matrices) );

			-- 预定义宏
			local preprocessor_defines = oc(def.preprocessor_defines,nil);
			if ( preprocessor_defines ) then
				if ( is_string(preprocessor_defines) ) then
					gp:setParameter("preprocessor_defines",preprocessor_defines);
				elseif ( is_table(preprocessor_defines ) ) then
					error("preprocessor_defines is talbe not support");
				else
					error("preprocessor_defines type not support");
				end;
			end;

			if ( gp:isLoaded() ) then
				gp:unload();
			end;
			
			gp:load(false);
		end

		local gpp = gp:getDefaultParameters();
		if ( gpp == nil ) then
			error(string.format("get program parameters failed:%s %s %s\n",source,entry_point,target));
		end;
		gpp:setIgnoreMissingParams(true);


		local named_auto_constants=oc(def.named_auto_constants,nil);
		if(named_auto_constants)then
			OgreMaterial.process_named_auto_constants(gpp,named_auto_constants,false);
		end;


		local named_auto_constants_real=oc(def.named_auto_constants_real,nil);
		if(named_auto_constants_real)then
			OgreMaterial.process_named_auto_constants(gpp,named_auto_constants_real,true);
		end;

		local named_constants=oc(def.named_constants,nil);
		if(named_constants)then
			OgreMaterial.process_named_constants(gpp,named_constants);
		end;


		table.foreachi(def,
			function(i,v)
				v(mt);
			end
		);
		
		local end_time=getTime_s();
		if(is_create_from_cached)then
			add_time(load_program_time,end_time-start_time,program_name);
		else
			add_time(compile_program_time,end_time-start_time,program_name);
		end;

	end
end

OgreMaterial.Program_Ref=function(def)
	_G.printf("%stemplate: Program_Ref ",space(3));

	local ret = option_call( def.mode, nil );
	
	copy_table_from_base(def);
	
	return function()
	
		_G.printf("%sProgram_Ref process",space(3));
		
		local oc = function(f,d)
			return option_call(f,d);
		end;
		if(oc(def.con,true)==false)then
			return;
		end;
		
		_G.printf("%sProgram_Ref process 1",space(3));
		
		local program_name = oc(def.program_name,nil);
		if(program_name==nil)then
			error("Program_Ref need name!!");
			return;
		end;
		
		_G.printf("%sProgram_Ref process 2",space(3));
		local program_type=oc(def.program_type,nil);	
		program_type = OgreMaterial.parse_program_type(program_type);	
		printf("%sProgram_Ref process 3",space(3));


		--local gpcm=Ogre.GpuProgramCacheManager.getSingletonPtr();
		local gm=Ogre.HighLevelGpuProgramManager.getSingletonPtr();
		local gp=gm:getByName(program_name);
		if gp == nil then
			error("Program_Ref not exists [" .. program_name .. "]" );
		end;
		
		_G.printf("%sProgram_Ref process 4",space(3));
		
		local gpp;
		if ( program_type == Ogre.Global.GPT_VERTEX_PROGRAM ) then
			context.pass:setVertexProgram(program_name,false);
			gpp=context.pass:getVertexProgramParameters();
		elseif ( program_type == Ogre.Global.GPT_FRAGMENT_PROGRAM ) then
			context.pass:setFragmentProgram(program_name,false);
			gpp=context.pass:getFragmentProgramParameters();
		end;
		
		if ( gpp == nil  ) then
			_G.printf("%sGet program parameters failed [%]",space(3),program_name);
			error("Get program parameters failed [" .. program_name .. "]" );
		end;
		
		_G.printf("%sProgram_Ref process 5",space(3));
		gpp:setIgnoreMissingParams(true);
		_G.printf("%sProgram_Ref process 6",space(3));
		
		local named_constants = oc(def.named_constants,nil);
		if ( named_constants ) then
			OgreMaterial.process_named_constants(gpp,named_constants);
		end;
		
		_G.printf("%sProgram_Ref process 7",space(3));
		
		local shared_params_ref = oc(def.shared_params_ref,nil);
		if ( shared_params_ref ) then
			if(is_string(shared_params_ref))then
				local gm2=Ogre.GpuProgramManager.getSingletonPtr();
				local sgp=gm2:getSharedParameters(shared_params_ref);
				if(sgp~=nil)then
					gpp:addSharedParameters(sgp:getName());
				end;
			elseif(is_table(shared_params_ref))then
				local gm2=Ogre.GpuProgramManager.getSingletonPtr();
				for i,v in ipairs(shared_params_ref) do
					if(is_string(v))then
						_G.printf("shared parameters %s",v);
						local sgp=gm2:getSharedParameters(v);
						_G.printf("add shared parameters %s",v);
						if(sgp~=nil and sgp.__ok==true)then
							_G.printf("add shared parameters %s begin",v);
							gpp:addSharedParameters(v);
							_G.printf("add shared parameters %s end",v);
						end;
					else
						error("Program_Ref share params ref table element need is string [" .. program_name .. "]" );
					end;
				end;
			else
				error("Program_Ref share params ref need string or table in [" .. program_name .. "]" );
			end;
		end;

		_G.printf("%sProgram_Ref process end",space(3));

		table.foreachi(def,
			function(i,v)
				v(mt);
			end
		);

	end
end

local PraseTextureType = function(s2)
	local texture_type=Ogre.Global.TEX_TYPE_2D;
	if(is_number(s2))then
		if(	s2==Ogre.Global.TEX_TYPE_1D or
			s2==Ogre.Global.TEX_TYPE_2D or
			s2==Ogre.Global.TEX_TYPE_3D or
			s2==Ogre.Global.TEX_TYPE_CUBE_MAP
			)then
			texture_type=s2;
		else
			error("Error Texture Type int["..tostring(s2).."]");
		end;
	elseif(is_string(s2))then
		local t=string.lower(s2);
		if(t=="1d")then
			texture_type=Ogre.Global.TEX_TYPE_1D;
		elseif(t=="2d")then
			texture_type=Ogre.Global.TEX_TYPE_2D;
		elseif(t=="3d")then
			texture_type=Ogre.Global.TEX_TYPE_3D;
		elseif(t=="cubic")then
			texture_type=Ogre.Global.TEX_TYPE_CUBE_MAP;
		else
			error("Error Texture Type string["..s2.."]" );
		end;
	else
		error("Error Texture Type :need int or string" );
	end;
	return texture_type;
end;

--tex_address_mode
local function parse_texture_address_mode(v)
	if(v=="wrap")then
		return Ogre.TextureUnitState.TAM_WRAP;
	elseif(v=="mirror")then
		return Ogre.TextureUnitState.TAM_MIRROR;
	elseif(v=="clamp")then
		return Ogre.TextureUnitState.TAM_CLAMP;
	elseif(v=="border")then
		return Ogre.TextureUnitState.TAM_BORDER;
	else
		error("Error texture address mode not type:["..v.."]");
	end;
end;

local set_tex_address_mode=function(ts,s1,s2,s3)
	if(is_number(s1))then
		if(s2==nil and s3==nil) then
			ts:setTextureAddressingMode(s1);
		elseif(s2 and s3)then
			ts:setTextureAddressingMode(s1,s2,s3);
		else
			error("Error tex_address_mode");
		end;
	elseif(is_string(s1))then
		if(s2==nil and s3==nil) then
			ts:setTextureAddressingMode(parse_texture_address_mode(s1));
		elseif(s2 and s3)then
			ts:setTextureAddressingMode(parse_texture_address_mode(s1),parse_texture_address_mode(s2),parse_texture_address_mode(s3));
		else
			error("Error tex_address_mode");
		end;
	else
		error("Error tex_address_mode,Only support string or int");
	end;
end;

OgreMaterial.TextureUnit=function(def)
	_G.printf("%stemplate: TextureUnit",space(3));

	local ret = option_call( def.mode, nil );
	
	copy_table_from_base(def);
	return function()

		local oc=function(f,d)
			return option_call(f,d);
		end

		if(oc(def.con,true)==false)then
			return;
		end

		local name=oc(def.name,"");
		_G.printf("%sTexture unit [%s]",space(3),name);

		local ts;


		local ts=oc(def.create,nil);
		if(ts==nil)then
			ts=context.pass:createTextureUnitState();
		end

		if(ts.__ok==false)then
			error("Create TextureUnit [%s] Failed in material [%s]",name,context.material:getName());
		end;

		context.texture_unit=ts;
		oc(def.after_create,nil);
		ts:setName(name);
		local texture_alias=oc(def.texture_alias,nil);
		if(texture_alias)then
			ts:setTextureNameAlias(texture_alias);
		end;

		if(context.samplers)then
			local sampler_register_name=oc(def.sampler_register_name,nil);
			if(sampler_register_name)then
				context.samplers[#context.samplers+1]=sampler_register_name;
			else
				error("texture unit not support sampler_register_name texutre=[" ..name .. "]",2);
			end;
		end;

		--texture
		--texture <texturename> [<type>] [unlimited | numMipMaps] [alpha] [<PixelFormat>] [gamma]
		local s1,s2,s3,s4,s5,s6=oc(def.texture,nil);
		if(s1)then
			local texture_name=s1;
			local texture_type=Ogre.Global.TEX_TYPE_2D;
			if(s2)then
				texture_type=PraseTextureType(s2);
			else
				local t_type=oc(def.type,nil);
				if(t_type)then
					texture_type=PraseTextureType(t_type);
				end;
			end;
			ts:setTextureName(texture_name,texture_type);
			--s3,s4,s5,s6
			--todo
		end;

		--anim_texture
		--anim_texture {"name1","name2"},<duration>
		--anim_texture {"name1","name2"},<duration>
		local s1,s2=oc(def.anim_texture,nil);
		if(s1)then
			if(is_table(s1))then
				ts:setAnimatedTextureName(s1,s2 or 1.0 );
			else
				error("Error anim_texture params");
			end;
		end;

		--content_type
		--string
		--int

		local s1=oc(def.content_type,nil);
		if(s1)then
			if ( s1==Ogre.TextureUnitState.CONTENT_NAMED or s1==Ogre.TextureUnitState.CONTENT_SHADOW )then
				ts:setContentType(s1);
			elseif(is_string(s1))then
				if(s1=="named")then
					ts:setContentType(Ogre.TextureUnitState.CONTENT_NAMED);
				elseif(s1=="shadow")then
					ts:setContentType(Ogre.TextureUnitState.CONTENT_SHADOW);
				elseif(s1=="compositor")then
				  ts:setContentType(Ogre.TextureUnitState.CONTENT_COMPOSITOR);
				  local s2=oc(def.compositorName,nil);
				  local s3=oc(def.compositorTextureName,nil);
				  local s4=oc(def.compositorTextureIndex,0);
				  if(s2 and s3) then
				    ts:setCompositorReference(s2,s3,s4);
				  end;
				else
					error("Error Content type");
				end;
			else
				error("Error Content type");
			end;
		end;

		--tex_coord_set
		local s1=oc(def.tex_coord_set ,nil);
		if(s1)then
			ts:setTextureCoordSet(s1);
		end;



		local s1,s2,s3=oc(def.tex_address_mode ,nil);

		local s1,s2,s3=oc(def.tex_address_mode ,nil);
		if(s1)then
			if(is_table(s1))then
				local s=s1;
				s1=s[1];
				s2=s[2];
				s3=s[3];
				set_tex_address_mode(ts,s1,s2,s3);
			elseif(is_number(s1) or is_string(s1))then
				set_tex_address_mode(ts,s1,s2,s3);
			else
				error("Error tex_address_mode,Only support string or int or table");
			end;
		end;

		--filtering
		--Format: filtering <none|bilinear|trilinear|anisotropic>
		--Format: filtering <minification> <magnification> <mip>
		local function parse_texture_filter_options(v)
			if(v=="none")then
				return Ogre.Global.TFO_NONE;
			elseif(v=="bilinear")then
				return Ogre.Global.TFO_BILINEAR;
			elseif(v=="trilinear")then
				return Ogre.Global.TFO_TRILINEAR;
			elseif(v=="anisotropic")then
				return Ogre.Global.TFO_ANISOTROPIC;
			else
				error("Error texture filter options :["..v.."]");
			end;
		end;
		local function parse_filter_options(v)
			if(v=="none")then
				return Ogre.Global.FO_NONE;
			elseif(v=="point")then
				return Ogre.Global.FO_POINT;
			elseif(v=="linear")then
				return Ogre.Global.FO_LINEAR;
			elseif(v=="anisotropic")then
				return Ogre.Global.FO_ANISOTROPIC;
			else
				error("Error filter options :["..v.."]");
			end;
		end;
		local s1,s2,s3=oc(def.filtering ,nil);
		if(s1)then
			if(is_number(s1))then
				if(s2==nil and s3==nil) then
					ts:setTextureFiltering(s1);
				elseif(s2 and s3)then
					ts:setTextureFiltering(s1,s2,s3);
				else
					error("Error filtering");
				end;
			elseif(is_string(s1))then
				if(s2==nil and s3==nil) then
					ts:setTextureFiltering(parse_texture_filter_options(s1));
				elseif(s2 and s3)then
					ts:setTextureFiltering(parse_filter_options(s1),parse_filter_options(s2),parse_filter_options(s3));
				else
					error("Error filtering");
				end;
			else
				error("Error filtering,Only support string or int");
			end;
		end;

		--max_anisotropy
		local s1=oc(def.max_anisotropy,nil);
		if(s1)then
			ts:setTextureAnisotropy(s1);
		end;

		--scroll_anim
		local s1,s2,s3=oc(def.scroll_anim,nil);
		if(s1)then
		    if(s3 == false or s3 == 0 or s3 == nil)then
			    ts:setScrollAnimation(s1,s2);
			else
			    ts:setTextureScroll(s1,s2,1.0);
            end;
		end;

		--rotate_anim
		local s1,s2=oc(def.rotate_anim,nil);
		if(s1)then
		    if(s2 == false or s2 == 0 or s2 == nil)then
			    ts:setRotateAnimation(s1);
			else
			    ts:setTextureRotate(Ogre.Radian(s1),Ogre.Vector3(0.0,0.0,1.0));
			end;
		end;
		
		-- scale anim
		local s1,s2,s3=oc(def.scale_anim,nil);
		if(s1)then
		    if(s3 == flase or s3 == 0 or s3 == nil)then
        	    ts:setScaleAnimation(s1,s2);
			else
			    ts:setTextureScale(s1,s2,1.0);
			end;
		end;

		_G.printf("%stexture unit",space(3));
		table.foreachi(def,
			function(i,v)
				v();
			end
		);
	end
end