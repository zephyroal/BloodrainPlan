#ifndef _JoyBasicMaterialDebugSwitch_H_
#define _JoyBasicMaterialDebugSwitch_H_

/*
		* 在这里添加静态编译用的宏定义
*/

#ifndef g_NoLighting
#define g_NoLighting 	0
#endif

#ifndef g_FogEnable
#define g_FogEnable		1
#endif

#ifndef g_UseSkin
#define g_UseSkin			0
#endif

#ifndef SD
#define SD						s0
#endif

#ifndef SD2
#define SD2						s1
#endif

#ifndef SNM
#define SNM						s2
#endif

#ifndef SBM
#define SBM						s3
#endif

#ifndef SCC
#define SCC						s4
#endif

#ifndef g_Diffuse2Enable
#define g_Diffuse2Enable 0
#endif

#ifndef g_Diffuse2BlendType
#define g_Diffuse2BlendType 0
#endif

#ifndef T_D_ScrollAnim_Enable
#define T_D_ScrollAnim_Enable 0
#define T_D_Anim_X 0
#define T_D_Anim_Y 0
#endif

#ifndef g_NormalMapEnable
#define g_NormalMapEnable 0
#endif

#ifndef g_SpecularMapEnable
#define g_SpecularMapEnable 0
#endif

#ifndef g_WeightPerBone
#define g_WeightPerBone 0
#endif

#ifndef g_UseVertexColor
#define g_UseVertexColor 0
#endif

#ifndef g_GrassEnable
#define g_GrassEnable 0
#endif

#endif
