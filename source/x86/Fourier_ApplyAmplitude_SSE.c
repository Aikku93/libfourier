/************************************************/
#ifndef __SSE__
# define __SSE__
#endif
/************************************************/
#include "Fourier.h"
#include "../Fourier_Helper.h"
/************************************************/
#if (defined(FOURIER_IS_X86) && defined(FOURIER_ALLOW_SSE))
/************************************************/

#include "../Fourier_ApplyAmplitude_Template.h"
void Fourier_ApplyAmplitude_SSE(float *Dst, const float *Src, const float *Amplitude, float DryLevel, int N) {
	Fourier_ApplyAmplitude_Template(Dst, Src, Amplitude, DryLevel, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
