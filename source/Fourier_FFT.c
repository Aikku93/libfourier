/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

// Declare specialized routines
void Fourier_FFT_Generic(float *Buf, float *Tmp, int N);
#ifdef FOURIER_IS_X86
# ifdef FOURIER_ALLOW_SSE
void Fourier_FFT_SSE    (float *Buf, float *Tmp, int N);
#  ifdef FOURIER_ALLOW_FMA
void Fourier_FFT_SSE_FMA(float *Buf, float *Tmp, int N);
#  endif
# endif
# ifdef FOURIER_ALLOW_AVX
void Fourier_FFT_AVX    (float *Buf, float *Tmp, int N);
#  ifdef FOURIER_ALLOW_FMA
void Fourier_FFT_AVX_FMA(float *Buf, float *Tmp, int N);
#  endif
# endif
#endif

/************************************************/

static const struct Fourier_FuncTbl_t DispatchTbl = {
	.Generic = Fourier_FFT_Generic,
#ifdef FOURIER_IS_X86
# ifdef FOURIER_ALLOW_SSE
	.SSE = Fourier_FFT_SSE,
#  ifdef FOURIER_ALLOW_FMA
	.SSE_FMA = Fourier_FFT_SSE_FMA,
#  endif
# endif
# ifdef FOURIER_ALLOW_AVX
	.AVX = Fourier_FFT_AVX,
#  ifdef FOURIER_ALLOW_FMA
	.AVX_FMA = Fourier_FFT_AVX_FMA,
#  endif
# endif
#endif
};

/************************************************/

typedef void (*Dispatcher_t)(float *Buf, float *Tmp, int N);
static void InitDispatcher(float *Buf, float *Tmp, int N);

static Dispatcher_t Dispatcher = InitDispatcher;

static void InitDispatcher(float *Buf, float *Tmp, int N) {
	Dispatcher = (Dispatcher_t)Fourier_GetDispatchFnc(&DispatchTbl);
	Dispatcher(Buf, Tmp, N);
}

/************************************************/

void Fourier_FFT(float *Buf, float *Tmp, int N) {
	Dispatcher(Buf, Tmp, N);
}

/************************************************/

// FFT (N=4)
void Fourier_FFT_4(float *x) {
	float x0r = x[0], x0i = x[1];
	float x1r = x[2], x1i = x[3];
	float x2r = x[4], x2i = x[5];
	float x3r = x[6], x3i = x[7];
	float a0r = x0r + x2r;
	float a0i = x0i + x2i;
	float b0r = x0r - x2r;
	float b0i = x0i - x2i;
	float a1r = x1r + x3r;
	float a1i = x1i + x3i;
	float b1r = x1r - x3r;
	float b1i = x1i - x3i;
	x[0] = a0r + a1r;
	x[1] = a0i + a1i;
	x[4] = a0r - a1r;
	x[5] = a0i - a1i;
	x[6] = b0r - b1i;
	x[7] = b0i + b1r;
	x[2] = b0r + b1i;
	x[3] = b0i - b1r;
}

/************************************************/

#include "Fourier_FFT_Template.h"
void Fourier_FFT_Generic(float *Buf, float *Tmp, int N) {
	Fourier_FFT_Template(Buf, Tmp, N);
}

/************************************************/
// EOF
/************************************************/
