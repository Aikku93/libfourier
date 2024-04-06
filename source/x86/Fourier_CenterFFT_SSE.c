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

#include "../Fourier_CenterFFT_Template.h"
void Fourier_FFTReCenter_SSE(float *Buf, float *Tmp, int N) {
	Fourier_FFTReCenter_Template(Buf, Tmp, N);
}
void Fourier_iFFTReCenter_SSE(float *Buf, float *Tmp, int N) {
	Fourier_iFFTReCenter_Template(Buf, Tmp, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
