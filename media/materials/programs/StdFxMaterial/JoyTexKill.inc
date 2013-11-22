#ifndef _JoyTexKill_INC_
#define _JoyTexKill_INC_

void Tex_Kill( inout FragParams params )
{
#ifdef _3DSMAX_
	if ( g_AlphaTestEnable )
	{
		clip( params.cFinal.a - g_AlphaTestValue );
	}
#else
	#if g_AlphaTestEnable == 1
		clip( params.cFinal.a - AlphaRef );
	#endif
#endif
}

#endif