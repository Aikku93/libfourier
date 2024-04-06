/************************************************/
#ifndef __AVX__
# define __AVX__
#endif
#ifndef __FMA__
# define __FMA__
#endif
/************************************************/
#include "Fourier.h"
#include "../Fourier_Helper.h"
/************************************************/
#if (defined(FOURIER_IS_X86) && defined(FOURIER_ALLOW_AVX) && defined(FOURIER_ALLOW_FMA))
/************************************************/

#include "../Fourier_ApplyAmplitude_Template.h"
void Fourier_ApplyAmplitude_AVX_FMA(float *Dst, const float *Src, const float *Amplitude, float DryLevel, int N) {
	Fourier_ApplyAmplitude_Template(Dst, Src, Amplitude, DryLevel, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
