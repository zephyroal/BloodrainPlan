/*  
@filename WXLightMapMaker.h   
@creator  LuoYinan  
@data     2010.6.14  
@remarks  创建场景光照图  
*/  
/*
#ifndef __WX_LIGHTMAP_MAKER_H__   
#define __WX_LIGHTMAP_MAKER_H__   

#include "SceneManipulator.h"   

namespace WX   
{   
	class LightMapMaker   
	{   
	public:   
		LightMapMaker(SceneManipulator* manipulator);   
		~LightMapMaker(void);   

		// 初始化，包括创建render texture，camera等   
		void initial(void);   
		// 销毁render texture   
		void destroy(void);   

		// 保存光照图到文件   
		// 格式 "emei.lightmap.png",也支持全路径,比如../../Data/Scene/emei.lightmap.png"   
		void outputTexture(const Ogre::String& fileName);   

	protected:   

		SceneManipulator* mSceneManipulator;   
		Ogre::RenderTexture* mRenderTexture;   

	};   
}   
#endif  
*/