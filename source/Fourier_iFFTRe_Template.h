/************************************************/
#define _USE_MATH_DEFINES // Sometimes needed by MSVC
#include <math.h>
/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

// This is the exact same as the real FFT routine, just run backwards
FOURIER_FORCE_INLINE
void Fourier_iFFTRe_Template(float *Buf, float *Tmp, int N) {
	int i;
	FOURIER_ASSUME_ALIGNED(Buf, FOURIER_ALIGNMENT);
	FOURIER_ASSUME_ALIGNED(Tmp, FOURIER_ALIGNMENT);
	FOURIER_ASSUME(N >= 8);

	// Begin "tangling"
	{
		float *BufLo = Buf;
		float *BufHi = Buf + N; // Buf + (N/2)*2
		float a0, a1, b0, b1;
		float s0, s1, d0, d1;
		float wc = cosf((float)(2.0*M_PI) / N);
		float ws = sinf((float)(2.0*M_PI) / N);
		float c  = wc;
		float s  = ws;
		a0 = BufLo[0];
		a1 = BufLo[1];
		*BufLo++ = a0 + a1;
		*BufLo++ = a0 - a1;
		for(i=1;i<N/4;i++) {
			BufHi -= 2;
			a0 = BufLo[0];
			a1 = BufLo[1];
			b0 = BufHi[0];
			b1 = BufHi[1];

			s0 = a0 + b0;
			a0 = a0 - b0;
			d1 = a1 - b1;
			a1 = a1 + b1;

			s1 = c*a0 - s*a1;
			d0 = s*a0 + c*a1;

			BufLo[0] = s0 - d0;
			BufLo[1] = s1 + d1;
			BufHi[0] = s0 + d0;
			BufHi[1] = s1 - d1;

			a0 = c;
			b0 = s;
			c = wc*a0 - ws*b0;
			s = ws*a0 + wc*b0;
			BufLo += 2;
		}
		BufLo[0] *= 2.0f;
		BufLo[1] *= 2.0f;
	}

	// Apply inverse FFT and "typecast" the data back to real
	Fourier_iFFT(Buf, Tmp, N/2);
}

/************************************************/
// EOF
/************************************************/
