#ifndef _JoyCodec_INC_
#define _JoyCodec_INC_

#ifndef CODEC_KEY
#define CODEC_KEY 256.f
#endif

/*
编码函数, 把一个R8G8B8A8的值转到一个float32位中,越往后精度越低
*/
void encode(in float4 color, out float codec)
{
	codec = color.x + 
					color.y * (1 / CODEC_KEY) + 
					color.z * (1 / (CODEC_KEY * CODEC_KEY)) + 
					color.w * (1 / (CODEC_KEY * CODEC_KEY * CODEC_KEY));
}

/*
	* 编码2个RGB8到1个RGB16中
*/
float3 encodeRGB8_16( in float3 iColorLow, in float3 iColorHigh )
{
		return floor(iColorLow*255)+(min(iColorHigh, 0.99));
}

void decodeRGB16_8( in float3 iColor, out float3 oColorLow, out float3 oColorHi )
{
		float3 tmp = floor( iColor );
		oColorLow = tmp / 255.f;
		oColorHi = iColor - tmp ;
}

/*
解码函数,从一个float32的数中得到R8G8B8A8某一位的值,越往后精度越低,解码速度越慢
*/
void decode_R(in float codec, out float r)	//R8
{
	r = floor(codec);
}

void decode_G(in float codec, out float g)	//G8
{
	codec = (codec - floor(codec)) * CODEC_KEY;
  g = floor(codec);
}

void decode_B(in float codec, out float b)	//B8
{
	codec = (codec - floor(codec)) * CODEC_KEY;
  codec = (codec - floor(codec)) * CODEC_KEY;
  b = floor(codec);
}

void decode_A(in float codec, out float a)	//A8
{
	codec = (codec - floor(codec)) * CODEC_KEY;
  codec = (codec - floor(codec)) * CODEC_KEY;
  codec = (codec - floor(codec)) * CODEC_KEY;
  a = floor(codec);
}

void decode_RG(in float codec, out float2 color)	//R8G8
{
	color.x = floor(codec);
	
	codec = (codec - color.x) * CODEC_KEY;
	color.y = floor(codec);
}

void decode_RGB(in float codec, out float3 color)	//R8G8B8
{
	color.x = floor(codec);
	
	codec = (codec - color.x) * CODEC_KEY;
	color.y = floor(codec);

	codec = (codec - color.y) * CODEC_KEY;
	color.z = floor(codec);
}

void decode_RGBA(in float codec, out float4 color)	//R8G8B8A8
{
	color.x = floor(codec);
	
	codec = (codec - color.x) * CODEC_KEY;
	color.y = floor(codec);

	codec = (codec - color.y) * CODEC_KEY;
	color.z = floor(codec);
	
	codec = (codec - color.z) * CODEC_KEY;
	color.w = floor(codec);
}

float2 encode_normal( float3 n )
{
		half p = sqrt(n.z*8+8);
    return n.xy/p + 0.5;

}

float3 decode_normal( float2 enc )
{
		half2 fenc = enc*4-2;
    half f = dot(fenc,fenc);
    half g = sqrt(1-f/4);
    half3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}

#endif