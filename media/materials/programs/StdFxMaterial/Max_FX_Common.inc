// note : max某些低版本不支持宏替换，暂时不使用
/*
#define RenderState(name) name = (g_##name);
#define TextureState(name,display_name) name=(g_##display_name);

#define _DefineRenderState(type,name,display_name,value) \
type g_##name < \
    string UIName = display_name; \
   > = value;

#define _DefineRenderState_MinMax(type,name,display_name,value,min_value,max_value,step) \
type g_##name < \
    string UIName = display_name; \
    int UIMin=min_value;\
    int UIMax=max_value;\
    int UIStep=step;\
   > = value;
*/

#define FLIP_TEXTURE_Y


float UnitScale(float v)
{
	return v/0.0254;
} 
float2 UnitScale(float2 v)
{
	return v/0.0254;
} 
float3 UnitScale(float3 v)
{
	return v/0.0254;
} 
float4 UnitScale(float4 v)
{
	return v/0.0254;
} 

#define _mul(a,b) mul(a,b)
