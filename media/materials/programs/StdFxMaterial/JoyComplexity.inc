#ifndef _JoyComplexity_INC_
#define _JoyComplexity_INC_

void Calc_Complexity_Color( inout FragParams params )
{
#if g_IsReleaseMode == 0
	params.cComplexity = tex1D(ComplexitySampler, ComplexityParams.x);
#endif
}

#endif