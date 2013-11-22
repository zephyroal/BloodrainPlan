--[[
-----------------------------------------------------------------------------
This Source File Is Part Of [Cross Platform Utility Library]
 
Copyright (c) winslylord@qtx All Rights Reserved
 
Mail				: winslylord@me.com
 
Description			: Lua Utility
 
Target Platform		: All
 
Ver					: 1.0.0
-----------------------------------------------------------------------------
--]]

Compositor = {};
Compositor.Compositor = function( def )

	_G.printf( "template: Compositor" );
	
	copy_table_from_base( def );
	
	return function( ct )
		local oc = function ( f, d )
			return option_call( f, d );
		end

		local f = getfenv(1);
		f.context = {};
		f.compositor_define = ct;

		local name = oc( def.name, "No_Name" );
		_G.printf( "start process compositor template: [%s]",name);

		-- 不开启则跳过
		if ( oc( def.con, true ) == false ) then
			_G.printf( "condition false");
			return;
		end

		local cm = Ogre.CompositorManager.getSingletonPtr();
		local c = cm:createOrRetrieve( name, "General" )

		if ( c == nil ) then
			_G.printf( "Create Compositor Failed![%s]",name );
		else
			_G.printf( "Create Compositor OK![%s]",name );
		end
		
		-- 首先清除所有默认生成的pass
		if ( c:getNumTechniques() > 0 ) then
			c:removeAllTechniques();
			c:unload()
		end

		context.compositor	=	c;
		context.technique	=	nil;
		context.target_ouput=	nil;
		context.target		=	{};
		context.texture		=	{};
		context.pass		=	nil;

		-- 提供调用时机
		oc( def.after_create, nil );

		table.foreachi(def,function(i,v)
			v();
		end)
	end
end

Compositor.Technique = function( def )

	_G.printf( "template: Compositor Techinque" );
	
	copy_table_from_base(def);
	
	return function()
	
		local oc = function(f,d)
			return option_call(f,d);
		end

		local scheme = oc( def.scheme, "" );
		printf( "start process compositor technique template: [%s]",scheme);

		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end

		local tec = context.compositor:createTechnique();
		context.technique = tec;
		
		oc( def.after_create, nil );

		tec:setSchemeName( scheme );

		table.foreachi(def,function(i,v)
			v();
		end)

		if ( context.technique.target_output == nil ) then
			-- error("must provider target_output!");
		end;
	end
end

-- depthBufferId 如果 == 0, 则不使用DepthBuffer
Compositor.Texture = function( def )

	_G.printf("template: Compositor Texture");
	
	copy_table_from_base(def);
	
	return function()
	
		local oc = function(f,d)
			return option_call(f,d);
		end

		if ( oc( def.con, true ) == false ) then
			_G.printf( "condition false");
			return;
		end

		--[[
				texture rt_gbuffer target_width target_height  	PF_FLOAT16_GR PF_R8G8B8A8 PF_R8G8B8A8
				texture rt_gbuffer target_width target_height  	PF_FLOAT16_RGBA PF_FLOAT16_RGBA PF_FLOAT16_RGBA
				texture rt_lbuffer target_width target_height 	PF_R8G8B8A8
		--]]

		local name = oc( def.name, nil );
		if ( name == nil ) then
			error( "Compositor texture define need name!" );
		end;
		_G.printf( "start process compositor texture template: [%s]",name);

		local size = oc( def.size, nil );
		if ( is_table(size) ) then
			if ( #size==2 ) then
				size[3]=1;
				size[4]=1;
			elseif (#size==4)then
			else
				error("Compositor texture define size table only supoort 2 or 4 field!");
			end;
		else
			error("Compositor texture define size field format error--need table!");
		end;

		local texture_format = oc( def.texture_format, nil );
		if ( texture_format == nil ) then
			error("Compositor texture define need texture_format!");
		end;

		local getTextureFormat = function( string_name )
			local t = "PF_".. string.upper(texture_format);
			t = Ogre.Global[t];
			if ( t == nil ) then
				error( "Compositor texture define need texture_format!" );
			end;
			return t;
		end;

		if ( is_string( texture_format ) ) then
			texture_format = getTextureFormat(texture_format);
			texture_format = {texture_format};
		elseif ( is_number(texture_format) ) then
			texture_format = {texture_format};
		elseif ( is_table(texture_format) ) then
			if ( #texture_format < 1 or #texture_format > 4 ) then
				error( "Compositor texture define texture_format only supoort 1-4 format!" );
			end;
			
			for k,v in ipairs(texture_format) do
				if ( is_string(v) ) then
					texture_format[k] = getTextureFormat(v);
				elseif ( not is_number(v) ) then
					error("Compositor texture define texture_format error:index:%d,value=%s!",k,tostring(v));
				end;
			end;
		else
			error("Compositor texture define texture_format error!");
		end;

		local pooled 		= oc(def.pooled,false);
		local gamma 		= oc(def.gamma,false);
		local fsaa			= oc(def.fsaa,true);
		local scope			= oc(def.scope,Ogre.CompositionTechnique.TS_LOCAL);
		local depthBufferId	= oc(def.depthBufferId,1);
		local refCompName	= oc(def.refCompName,nil);
		local refTexName	= oc(def.refTexName,nil);

		local td 		= context.technique:createTextureDefinition(name);
		td.width		= size[1];
		td.height		= size[2];
		td.widthFactor	= size[3];
		td.heightFactor	= size[4];

		for k,v in ipairs( texture_format ) do
			-- 在这里进行HDR贴图切换
			if ( Ogre.CompositorManager.getSingletonPtr():testCustomMask(Ogre.FAIRY.Mask_HDR) ) then
				if ( v == Ogre.Global.PF_R8G8B8A8 or v == Ogre.Global.PF_A8R8G8B8 ) then
					v = Ogre.Global.PF_FLOAT16_RGBA;
				end;
			end;
			td:addTextureDef(v);
		end;

		td.fsaa				= fsaa;
		td.hwGammaWrite		= gamma;
		td.pooled			= pooled;
		td.scope			= scope;
		td.depthBufferId 	= depthBufferId;
		if ( refCompName ) then
			td.refCompName	= refCompName;
		end;
		if ( refTexName ) then
			td.refTexName	= refTexName;
		end;

		oc( def.after_create, nil );
		table.foreachi(def,function(i,v)
			v();
		end)
	end
end


Compositor.Target = function( def )

	_G.printf( "template: Compositor Target" );
	
	copy_table_from_base(def);
	
	return function()
	
		local oc=function(f,d)
			return option_call(f,d);
		end
		
		if ( oc(def.con, true ) == false ) then
			printf( "condition false");
			return;
		end

		_G.printf( "start process compositor target");

		local target;

		local name = oc( def.name, nil );
		if ( name == nil or name == "" ) then
			if( context.technique.target_output ~= nil ) then
				--error("Only one target output for the technique enabled!");
				return;
			end;
			_G.printf( "start process compositor target");
			-- 暂时使用改过的Ogre后处理版本，这里不指定输出pass，改为指定OUTPUT类型
			target = context.technique:createTargetPass();
			context.technique.target_output=target;
		else
			target=context.technique:createTargetPass();
			target:setOutputName(name);
		end;

		table.insert(context.target,target);

		oc(def.after_create,nil);


		-- input 相关的内容
		-- input (none | previous)
		-- Ogre.CompositionTargetPass.IM_NONE
		-- Ogre.CompositionTargetPass.IM_PREVIOUS
		local target_input = oc( def.input, Ogre.CompositionTargetPass.IM_NONE );
		if( is_string(target_input) ) then
			local t = Ogre.CompositionTargetPass["IM_".. string.upper(target_input)];
			if ( t == nil ) then
				error("Compositor target pass input erorr,input is "..target_input);
			end;
			target_input = t;
		elseif( is_number( target_input ) ) then
			if(	target_input~=Ogre.CompositionTargetPass.IM_NONE and
				target_input~=Ogre.CompositionTargetPass.IM_PREVIOUS
			)then
				error("Compositor target pass input erorr,input is [" .. target_input .. "]");
			end;
		else
			error("Compositor target pass input erorr,input need number or string, got: [" .. type(target_input) .. "]");
		end;
		
		target:setInputMode(target_input);
		
		-- output 配合compositor的更改
		-- 不推荐设置输出模式
		local output = oc( def.output, Ogre.CompositionTargetPass.OM_NONE );
		target:setOutputMode( output );
		
		-- priority
		local pri = oc( def.priority, 255 );
		target:setPriority( pri );

		--only_initial
		local only_initial = oc(def.only_initial,false);
		target:setOnlyInitial(only_initial);

		--visibility_mask
		local visibility_mask = oc(def.visibility_mask,nil);
		if(visibility_mask)then
			target:setVisibilityMask(visibility_mask);
		end;
		
		--visibility_mask_op
		local visibility_mask_op = oc(def.visibility_mask_op,nil);
		if(visibility_mask_op)then
			target:setVisibilityMask_op(visibility_mask_op);
		end;

		--material_scheme
		local material_scheme = oc(def.material_scheme,nil);
		if(material_scheme)then
			target:setMaterialScheme(material_scheme);
		end;

		--lod_bias
		local lod_bias = oc(def.lod_bias,nil);
		if(lod_bias)then
			target:setLodBias(lod_bias);
		end;

		local shadows = oc(def.shadows,nil);
		if(shadows)then
			shadows=parse_boolean(shadows);
			target:setShadowsEnabled(shadows);
		end;
		
		local fog = oc(def.fog,nil);
		if(fog)then
			fog=parse_boolean(fog);
			target:setShadowsEnabled(fog);
		end;
		
		--- shared render states
		local id = oc(def.id,Ogre.FAIRY.PassID_Unknown);
		if(id)then
			target:setPassId(id);
		end;
		
		local blend_src = oc(def.blend_src,nil);
		local blend_dst = oc(def.blend_dst,nil);
		if(blend_src~=nil and blend_dst~=nil)then
			target:setSharedBlendState(blend_src,blend_dst);
		end;
		
		local addr_mode = oc(def.addr_mode,nil);
		if(addr_mode~=nil)then
			target:setSharedAddrMode(addr_mode);
		end;
		
		local filter_option = oc(def.filter_option,nil);
		if(filter_option~=nil)then
			target:setSharedFilterOption(filter_option);
		end;
		
		local max_aniso = oc(def.max_aniso,nil);
		if(max_aniso~=nil)then
			target:setSharedMaxAnisotropy(max_aniso);
		end;
		
		local mipmap_bias = oc(def.mipmap_bias,nil);
		if(mipmap_bias~=nil)then
			target:setSharedMipmapBias(mipmap_bias);
		end;
		
		local depth_fun = oc(def.depth_fun,nil);
		if(depth_fun~=nil)then
			target:setSharedDepthFunc(depth_fun);
		end;
		
		local depth_write = oc(def.depth_write,nil);
		if(depth_write~=nil)then
			target:setSharedDepthWrite(depth_write);
		end;
		
		local depth_check = oc(def.depth_check,nil);
		if(depth_check~=nil)then
			target:setSharedDepthCheck(depth_check);
		end;
		
		local depth_bias = oc(def.depth_bias,nil);
		if(depth_bias~=nil)then
			target:setSharedDepthBias(depth_bias);
		end;
		
		local alpha_fun = oc(def.alpha_fun,nil);
		if(alpha_fun~=nil)then
			target:setSharedAlphaRejectFun(alpha_fun);
		end;
		
		local alpha_val = oc(def.alpha_val,nil);
		if(alpha_val~=nil)then
			target:setSharedAlphaRejectVal(alpha_val);
		end;

		table.foreachi(def,function(i,v)
			v();
		end)
	end
end

Compositor.Clear = function(def)

	_G.printf("template: Compositor Pass Clear");
	
	copy_table_from_base(def);
	
	return function()
	
		local oc=function(f,d)
			return option_call(f,d);
		end
		
		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end
		
		oc(def.after_create,nil);

		_G.printf( "start process compositor target pass clear");
		
		local target = context.target[#context.target];
		local pass = target:createPass();
		pass:setType(Ogre.CompositionPass.PT_CLEAR);
		context.pass=pass;

		--identifier
		local identifier = oc(def.identifier ,nil);
		if( identifier )then
			pass:setIdentifier(identifier);
		end;

		--buffers
		local buffers = oc(def.buffers,nil);
		if ( buffers == nil ) then
			pass:setClearBuffers(Ogre.Global.FBT_COLOUR+Ogre.Global.FBT_DEPTH);
		elseif(is_table(buffers))then
			local b = 0;
			for k,v in ipairs(buffers) do
				if(is_number(v))then
					b=b+v;
				elseif(is_string(v))then
					local t=Ogre.Global["FBT_" .. string.upper(v)];
					if(t==nil)then
						error("Compositor Pass Clear buffers value error![".. v .."]" );
					end;
					b=b+t;
				else
					error("Compositor Pass Clear buffers value need Ogre.FBT_.... or string![".. v .."]" );
				end;
			end;
			pass:setClearBuffers(b);
		else
			error("Compositor Pass Clear buffers should is table!");
		end;

		--colour_value
		local colour_value = oc(def.colour_value,nil);
		if(colour_value)then
			pass:setClearColour(colour_value);
		end;

		--depth_value
		local depth_value = oc(def.depth_value,nil);
		if(depth_value)then
			pass:setClearDepth(depth_value);
		end;

		--stencil_value
		local stencil_value = oc(def.stencil_value,nil);
		if(stencil_value)then
			pass:setClearStencil(stencil_value);
		end;

		table.foreachi(def,function(i,v)
			v();
		end)
	end
end

Compositor.Stencil = function(def)

	_G.printf("template: Compositor Pass Stencil");
	
	copy_table_from_base(def);
	
	return function()
		local oc=function(f,d)
			return option_call(f,d);
		end
		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end
		
		oc(def.after_create,nil);

		_G.printf( "start process compositor target pass stencil");

		local target = context.target[#context.target];
		local pass = target:createPass();
		pass:setType(Ogre.CompositionPass.PT_STENCIL);
		context.pass=pass;

		--identifier
		local identifier = oc(def.identifier ,nil);
		if(identifier )then
			pass:setIdentifier(identifier);
		end;

		--check
		local check = oc(def.check ,nil);
		if(check )then
			check=parse_boolean(check);
			pass:setStencilCheck(check);
		end;

		--comp_func
		local comp_func = oc(def.comp_func ,nil);
		if(comp_func )then
			if( is_number(comp_func))then
				pass:setStencilFunc(comp_func);
			elseif( is_string(comp_func))then
				local v=Ogre["CMPF_".. string.upper(comp_func) ];
				if(v==nil)then
					error("Compare function error: [" .. comp_func .. "]");
				end;
				pass:setStencilFunc(v);
			else
				error("Compositor Stencil pass comp_func value error: [" .. tostring(comp_func) .. "]");
			end;
		end;


		--ref_value
		local ref_value = oc(def.ref_value ,nil);
		if(ref_value )then
			pass:setStencilRefValue(ref_value);
		end;

		--mask
		local mask = oc(def.mask ,nil);
		if(mask )then
			pass:setStencilMask(mask);
		end;


		local parse_sop=function(v)
			if( is_number(v))then
				return v;
			elseif( is_string(v))then
				local t=Ogre["SOP_".. string.upper(v) ];
				if(t==nil)then
					error("Stencil operator error: [" .. v .. "]");
				end;
				return t;
			else
				error("Stencil operator error: [" .. tostring(v) .. "]");
			end;
		end;

		--fail_op
		local fail_op = oc(def.fail_op ,nil);
		if(fail_op )then
			local v=parse_sop(fail_op);
			pass:setStencilFailOp(v);
		end;

		--depth_fail_op
		local depth_fail_op = oc(def.depth_fail_op ,nil);
		if(depth_fail_op )then
			local v=parse_sop(depth_fail_op);
			pass:setStencilDepthFailOp(v);
		end;

		--pass_op
		local pass_op = oc(def.pass_op ,nil);
		if(pass_op )then
			local v=parse_sop(pass_op);
			pass:setStencilPassOp(v);
		end;

		--two_sided
		local two_sided = oc(def.two_sided ,nil);
		if(two_sided )then
			check=parse_boolean(two_sided);
			pass:setStencilTwoSidedOperation(two_sided);
		end;

		table.foreachi(def,function(i,v)
			v();
		end)
	end
end



Compositor.RenderScene = function(def)

	printf("template: Compositor Pass RenderScene");
	
	copy_table_from_base(def);
	
	return function()
		local oc=function(f,d)
			return option_call(f,d);
		end
		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end
		
		oc(def.after_create,nil);

		_G.printf( "start process compositor target pass RenderScene");

		local target = context.target[#context.target];
		local pass = target:createPass();
		pass:setType( Ogre.CompositionPass.PT_RENDERSCENE );
		context.pass=pass;

		--identifier
		local identifier = oc(def.identifier ,nil);
		if( identifier )then
			pass:setIdentifier(identifier);
		end;

		--first_render_queue
		local first_render_queue = oc(def.first_render_queue ,nil);
		if(first_render_queue )then
			pass:setFirstRenderQueue(first_render_queue);
		end;

		--last_render_queue
		local last_render_queue = oc(def.last_render_queue ,nil);
		if(last_render_queue )then
			pass:setLastRenderQueue(last_render_queue);
		end;

		table.foreachi(def,function(i,v)
			v(context);
		end)
	end
end

Compositor.ReflectionRender = function(def)
	printf("template: Compositor Pass ReflectionRender");
	copy_table_from_base(def);
	return function()
		local oc=function(f,d)
			return option_call(f,d);
		end
		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end
		oc(def.after_create,nil);

		printf( "start process compositor target pass RenderScene");

		local target = context.target[#context.target];
		local pass = target:createPass();
		pass:setType(Ogre.CompositionPass.PT_RENDERCUSTOM);
		pass:setCustomType("reflection");
		context.pass=pass;

	end
end

Compositor.DLCPRender = function(def)
	printf("template: Compositor Pass DLCPRender");
	copy_table_from_base(def);
	return function()
		local oc=function(f,d)
			return option_call(f,d);
		end
		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end
		oc(def.after_create,nil);

		printf( "start process compositor target pass RenderScene");

		local target = context.target[#context.target];
		local pass = target:createPass();
		pass:setType(Ogre.CompositionPass.PT_RENDERCUSTOM);
		pass:setCustomType("DLCP");
		context.pass=pass;

	end
end

Compositor.RenderQuad = function(def)

	_G.printf("template: Compositor Pass RenderQuad");
	
	copy_table_from_base(def);
	
	return function()
		local oc=function(f,d)
			return option_call(f,d);
		end
		
		if(oc(def.con,true)==false)then
			printf( "condition false");
			return;
		end
		
		oc(def.after_create,nil);

		printf( "start process compositor target pass RenderQuad");

		local target=context.target[#context.target];
		local pass=target:createPass();
		pass:setType(Ogre.CompositionPass.PT_RENDERQUAD);
		context.pass=pass;

		--identifier
		local identifier = oc(def.identifier ,nil);
		if(identifier )then
			pass:setIdentifier(identifier);
		end;

		--material
		local material  = oc(def.material  ,nil);
		if(material  )then
			pass:setMaterialName( material );
		end;

		--inputs
		--inputs={ "rt1",{"rt1",1},"rt3"};
		local inputs =oc(def.inputs ,nil);
		if(inputs==nil)then
		elseif( is_table(inputs)  )then
			for k,v in ipairs(inputs) do
				--表中的每一行还应该是一个表或者字符串
				if(is_table(v))then
					if(not is_string(v[1]))then
						error(string.format("RenderQuad's input need texture name (string),Compostor[%s],technique[%s]",
							context.compositor:getName(),context.technique:getSchemeName()) );
					end;
					pass:setInput(k-1,v[1],v[2]);
				elseif(is_string(v))then
					pass:setInput(k-1,v,0);
				else
					error("Compositor Target pass Render Quad Inputs value error! each input need a string or {name,index}!");
				end;
			end;
		else
			error("Compositor Target pass Render Quad Inputs value error! need table!");
		end;

		table.foreachi(def,function(i,v)
			v();
		end)
	end
end

