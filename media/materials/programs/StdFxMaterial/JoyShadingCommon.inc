#ifndef _JoyShadingCommon_INC_
#define _JoyShadingCommon_INC_

/// Common Shading Option Here


/// 临时关闭草的交互动画
//#define g_GrassEnable 0

/// 处理mul
#ifdef _3DSMAX_
#define MUL(x,y) mul(y,x)
#else
#define MUL(x,y) mul(x,y)
#endif

#if g_SM3 != 1
#define tex2Dlod(a,b) tex2D(a,b.xy)
#endif

#endif