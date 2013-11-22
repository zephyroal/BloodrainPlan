
void SceneBrightAdjust_PS(
             float2 texCoord : TexCoord0,	             
             out float4 color : COLOR,             
    				 uniform sampler2D SceneTex : register(s0),    				 
    				 uniform float brightScale,
    				 uniform float4 viewportSize
             )
{  
		texCoord = texCoord + 0.5 * viewportSize.zw;
    float4 scnColor = tex2D(SceneTex, texCoord);
    color.rgb = scnColor.rgb * brightScale;
    color.a   = scnColor.a;  
}