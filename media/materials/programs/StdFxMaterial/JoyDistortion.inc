#ifndef _JoyDistort_INC_
#define _JoyDistort_INC_

void Distortion( inout FragParams params )
{
#if g_TexDistortionEnable == 1
  params.FI.baseTC.xy += tex2D(DistortionSampler, params.FI.baseTC.zw)  * DistortionPower;
#endif

#if g_SpaceDistortionEnable == 1
  params.FI.extTC2.xy /= params.FI.extTC2.w;
  float2 origin = params.FI.extTC2.xy;
  float4 offset = tex2D(DistortionSampler, params.FI.baseTC.zw);
  
  params.FI.extTC2.xy += offset.xy * params.FI.color.xy * DistortionPower;
  params.FI.extTC2.z = offset.z > 0.0f ? 1.0f : 0.0f;
  
  float depth = tex2D(SceneSampler, params.FI.extTC2.xy).w;
  if(depth < params.FI.extTC3.x)
  {
  	params.FI.extTC2.xy = origin;
  }
#endif

}


#endif