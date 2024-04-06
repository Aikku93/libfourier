/************************************************/
#ifndef __SSE__
# define __SSE__
#endif
#ifndef __FMA__
# define __FMA__
#endif
/************************************************/
#include "Fourier.h"
#include "../Fourier_Helper.h"
/************************************************/
#if (defined(FOURIER_IS_X86) && defined(FOURIER_ALLOW_SSE) && defined(FOURIER_ALLOW_FMA))
/************************************************/

#include "../Fourier_CenterFFT_Template.h"
void Fourier_FFTReCenter_SSE_FMA(float *Buf, float *Tmp, int N) {
	Fourier_FFTReCenter_Template(Buf, Tmp, N);
}
void Fourier_iFFTReCenter_SSE_FMA(float *Buf, float *Tmp, int N) {
	Fourier_iFFTReCenter_Template(Buf, Tmp, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
