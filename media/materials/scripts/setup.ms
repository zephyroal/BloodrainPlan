if ( Ogre.Util.lua_execute == nil ) then
	error( "初始化lua环境失败" );
end;

execute = function( script )
	Ogre.Util.lua_execute( script, "Script" );
end;

-- 是否启用材质转换
SetupConfig =
{
		g_EnableMaterialAutoConvert = 1;
};

-- 材质脚本解析时的回调
ParseCache = function(p)
	Ogre.MaterialManager.getSingletonPtr():ParseCache( p );
end;

-- 默认的hash函数
Ogre.Util.lua_setPassHashFunc(0);

-- 前项渲染支持的最大光源数量
Max_Light_Num = 4;

-- 启动后项执行的文件
execute("Utility.ms");
execute("MaterialCommon.ms");
execute("Material.ms");
execute("JoyBasicMaterial.ms");
execute("JoyTerrainMaterial.ms");
execute("JoyParticleMaterial.ms");
execute("JoyLightMaterial.ms");

-- 自定义compositor文件
execute("Compositor.lua");
execute("DefaultCompositor_Material.lua");

-- 编辑器渲染控制开关
gpm = Ogre.GpuProgramManager.getSingletonPtr();
gpp = gpm:createSharedParameters( "EditorControl" );
gpp:addConstantDefinition( "EditorParams", 4, 1 );
gpp:setNamedConstant( "EditorParams", Ogre.Vector4(0,1.0,0,0) );

-- 场景人物光照控制
gpp:addConstantDefinition( "ActorParams", 4, 1 );
gpp:setNamedConstant( "ActorParams", Ogre.Vector4(1.0, 1.0, 1.0, 1.0) );

-- 场景复杂度颜色
gpp:addConstantDefinition( "ComplexityParams", 4, 1 );
gpp:setNamedConstant( "ComplexityParams", Ogre.Vector4(0.0, 0.0, 0.0, 0.0) );

-- 全局渲染模式
g_Render_Mode = nil;

-- 是否是编辑器模式
g_IsEditor = false;