#ifndef _JoyLightModel_INC_
#define _JoyLightModel_INC_

/// optimized phong, use if mirrowed reflection vector pre-computed
float phong_impl( float3 r, float3 l, float exp )						// 5 ALU
{
		float fNormFactor = exp * 0.159155f + 0.31831f; 				// 1 ALU
		return fNormFactor * pow( saturate(dot(l,r)), exp );		// 4 ALU
}

float phone( float3 n, float3 v, float3 l, float exp )			// 8 ALU
{
		float3 r = reflect( -v, n );														// 3 ALU
		return phong_impl( r, l, exp );													// 5 ALU
}

float blinn( float3 n, float3 v, float3 l, float exp )			// 7 ALU
{
		float fNormFactor = exp * 0.159155h + 0.31831h;					// 1 ALU
		float3 h = normalize(v + l);														// 2 ALU
		return fNormFactor * pow( saturate(dot(n,h)), exp );		// 4 ALU
}

float bank( float3 n, float3 v, float3 l, float exp )
{
		float3 tangentDir = normalize( cross(n,l) );
		float dotnl = dot(n,l);
		float dotlt = dot(l,tangentDir);
		float dotvt = dot(v,tangentDir);
		return pow( sqrt(1-dotlt*dotlt)*sqrt(1-dotvt*dotvt), exp ) * dotnl;
}

float simpleBRDF( float3 n, float3 v, float3 l, float exp )	// 7 ALU
{
		// phone or blinn
		return blinn( n, v, l, exp );
}

float3 normalCalc( float3 mapNorm, float bumpScale )				// 3 ALU
{
		float3 v = {0.5f,0.5f,1.0f};														// 1 ALU
		mapNorm = lerp( v, mapNorm, bumpScale );								// 1 ALU
		mapNorm = ( mapNorm * 2.0f ) - 1.0f;										// 1 ALU
		return mapNorm;
}

#endif