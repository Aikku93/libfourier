/************************************************/
#ifndef __AVX__
# define __AVX__
#endif
/************************************************/
#include "Fourier.h"
#include "../Fourier_Helper.h"
/************************************************/
#if (defined(FOURIER_IS_X86) && defined(FOURIER_ALLOW_AVX))
/************************************************/

#include "../Fourier_CenterFFT_Template.h"
void Fourier_FFTReCenter_AVX(float *Buf, float *Tmp, int N) {
	Fourier_FFTReCenter_Template(Buf, Tmp, N);
}
void Fourier_iFFTReCenter_AVX(float *Buf, float *Tmp, int N) {
	Fourier_iFFTReCenter_Template(Buf, Tmp, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
