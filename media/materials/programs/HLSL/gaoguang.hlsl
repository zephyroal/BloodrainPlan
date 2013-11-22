sampler2D texColor : register(s0);
sampler2D lightColor : register(s1);
void
mainPS(
    in float2 uv : TEXCOORD0,
    in float4 diffuse : COLOR0,
    in float4 specular : COLOR1,
    out float4 oColour : COLOR)
{
    float4 color = tex2D(texColor, uv);
    float3 texColor = color.rgb;
    float4 baseColour = diffuse;
    float specularFactor = tex2D(lightColor, uv);
    float3 finalColour = baseColour.rgb* texColor + specular.rgb * specularFactor;
    oColour = float4(finalColour, baseColour.a);
}
