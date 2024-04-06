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

#include "../Fourier_ApplyWindow_Template.h"
void Fourier_ApplyWindow_AVX_FMA(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_ApplyWindow_Template(Dst, Src, Window, N);
}
void Fourier_OverlapWindow_AVX_FMA(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_OverlapWindow_Template(Dst, Src, Window, N);
}
void Fourier_ApplySymmetricWindow_AVX_FMA(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_ApplySymmetricWindow_Template(Dst, Src, Window, N);
}
void Fourier_OverlapSymmetricWindow_AVX_FMA(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_OverlapSymmetricWindow_Template(Dst, Src, Window, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
