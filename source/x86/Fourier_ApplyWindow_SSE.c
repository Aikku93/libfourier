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

#include "../Fourier_ApplyWindow_Template.h"
void Fourier_ApplyWindow_SSE(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_ApplyWindow_Template(Dst, Src, Window, N);
}
void Fourier_OverlapWindow_SSE(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_OverlapWindow_Template(Dst, Src, Window, N);
}
void Fourier_ApplySymmetricWindow_SSE(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_ApplySymmetricWindow_Template(Dst, Src, Window, N);
}
void Fourier_OverlapSymmetricWindow_SSE(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_OverlapSymmetricWindow_Template(Dst, Src, Window, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
