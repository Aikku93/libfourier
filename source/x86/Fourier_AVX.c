/************************************************/
#ifndef __AVX__
# define __AVX__ // MSVC hack
#endif
/************************************************/
#include "Fourier.h"
#include "../Fourier_Helper.h"
/************************************************/
#if (defined(FOURIER_IS_X86) && defined(FOURIER_ALLOW_AVX))
/************************************************/

#include "../Fourier_DCT2_Template.h"
void Fourier_DCT2_AVX(float *Buf, float *Tmp, int N) {
	Fourier_DCT2_Template(Buf, Tmp, N);
}

#include "../Fourier_DCT4_Template.h"
void Fourier_DCT4_AVX(float *Buf, float *Tmp, int N) {
	Fourier_DCT4_Template(Buf, Tmp, N);
}

#include "../Fourier_FFT_Template.h"
void Fourier_FFT_AVX(float *Buf, float *Tmp, int N) {
	Fourier_FFT_Template(Buf, Tmp, N);
}

#include "../Fourier_iFFT_Template.h"
void Fourier_iFFT_AVX(float *Buf, float *Tmp, int N) {
	Fourier_iFFT_Template(Buf, Tmp, N);
}

#include "../Fourier_CenterFFT_Template.h"
void Fourier_FFTReCenter_AVX(float *Buf, float *Tmp, int N) {
	Fourier_FFTReCenter_Template(Buf, Tmp, N);
}
void Fourier_iFFTReCenter_AVX(float *Buf, float *Tmp, int N) {
	Fourier_iFFTReCenter_Template(Buf, Tmp, N);
}

#include "../Fourier_MDCT_Template.h"
void Fourier_MDCT_MDST_AVX(float *MDCT, float *MDST, const float *New, float *Lap, float *BufTmp, int N, int Overlap) {
	Fourier_MDCT_MDST_Template(MDCT, MDST, New, Lap, BufTmp, N, Overlap);
}

#include "../Fourier_IMDCT_Template.h"
void Fourier_IMDCT_AVX(float *BufOut, const float *BufIn, float *BufLap, float *BufTmp, int N, int Overlap) {
	Fourier_IMDCT_Template(BufOut, BufIn, BufLap, BufTmp, N, Overlap);
}

#include "../Fourier_ApplyAmplitude_Template.h"
void Fourier_ApplyAmplitude_AVX(float *Dst, const float *Src, const float *Amplitude, float DryLevel, int N) {
	Fourier_ApplyAmplitude_Template(Dst, Src, Amplitude, DryLevel, N);
}

#include "../Fourier_ApplyWindow_Template.h"
void Fourier_ApplyWindow_AVX(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_ApplyWindow_Template(Dst, Src, Window, N);
}
void Fourier_OverlapWindow_AVX(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_OverlapWindow_Template(Dst, Src, Window, N);
}
void Fourier_ApplySymmetricWindow_AVX(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_ApplySymmetricWindow_Template(Dst, Src, Window, N);
}
void Fourier_OverlapSymmetricWindow_AVX(float *Dst, const float *Src, const float *Window, int N) {
	Fourier_OverlapSymmetricWindow_Template(Dst, Src, Window, N);
}

/************************************************/
#endif
/************************************************/
// EOF
/************************************************/
