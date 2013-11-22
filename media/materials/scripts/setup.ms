if ( Ogre.Util.lua_execute == nil ) then
	error( "��ʼ��lua����ʧ��" );
end;

execute = function( script )
	Ogre.Util.lua_execute( script, "Script" );
end;

-- �Ƿ����ò���ת��
SetupConfig =
{
		g_EnableMaterialAutoConvert = 1;
};

-- ���ʽű�����ʱ�Ļص�
ParseCache = function(p)
	Ogre.MaterialManager.getSingletonPtr():ParseCache( p );
end;

-- Ĭ�ϵ�hash����
Ogre.Util.lua_setPassHashFunc(0);

-- ǰ����Ⱦ֧�ֵ�����Դ����
Max_Light_Num = 4;

-- ��������ִ�е��ļ�
execute("Utility.ms");
execute("MaterialCommon.ms");
execute("Material.ms");
execute("JoyBasicMaterial.ms");
execute("JoyTerrainMaterial.ms");
execute("JoyParticleMaterial.ms");
execute("JoyLightMaterial.ms");

-- �Զ���compositor�ļ�
execute("Compositor.lua");
execute("DefaultCompositor_Material.lua");

-- �༭����Ⱦ���ƿ���
gpm = Ogre.GpuProgramManager.getSingletonPtr();
gpp = gpm:createSharedParameters( "EditorControl" );
gpp:addConstantDefinition( "EditorParams", 4, 1 );
gpp:setNamedConstant( "EditorParams", Ogre.Vector4(0,1.0,0,0) );

-- ����������տ���
gpp:addConstantDefinition( "ActorParams", 4, 1 );
gpp:setNamedConstant( "ActorParams", Ogre.Vector4(1.0, 1.0, 1.0, 1.0) );

-- �������Ӷ���ɫ
gpp:addConstantDefinition( "ComplexityParams", 4, 1 );
gpp:setNamedConstant( "ComplexityParams", Ogre.Vector4(0.0, 0.0, 0.0, 0.0) );

-- ȫ����Ⱦģʽ
g_Render_Mode = nil;

-- �Ƿ��Ǳ༭��ģʽ
g_IsEditor = false;