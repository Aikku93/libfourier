/************************************************/
#define _USE_MATH_DEFINES // Sometimes needed by MSVC
#include <math.h>
/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

FOURIER_FORCE_INLINE
void Fourier_FFTRe_Template(float *Buf, float *Tmp, int N) {
	int i;
	FOURIER_ASSUME_ALIGNED(Buf, FOURIER_ALIGNMENT);
	FOURIER_ASSUME_ALIGNED(Tmp, FOURIER_ALIGNMENT);
	FOURIER_ASSUME(N >= 8);

	// "Typecast" the data as complex and take Z=DFT[Buf]
	Fourier_FFT(Buf, Tmp, N/2);

	// Begin "untangling"
	// Source: http://www.robinscheibler.org/2013/02/13/real-fft.html
	// Further symmetry derivations by me below:
	/*

	Start with:
	 Z = DFT[Buf.AsInterleavedComplex()]

	For k=1..N/2-1:
	 Xe[k] =    (Z[k] + Z*[N/2-k])/2
	 Xo[k] = -I*(Z[k] - Z*[N/2-k])/2
	 X[k]  = Xe[k] + Xo[k]*w

	Now expanding on some symmetries:
	 w[N/2-k]  = -Conjugate[w[k]]
	 Xe[k]     =    (Z[k] + Conjugate[Z[N/2-k]])/2
	 Xo[k]     = -I*(Z[k] - Conjugate[Z[N/2-k]])/2
	 X[k]      = Xe[k] + Xo[k]*w
	 Xe[N/2-k] =    (Z[N/2-k] + Conjugate[Z[k]])/2
	 Xo[N/2-k] = -I*(Z[N/2-k] - Conjugate[Z[k]])/2
	 X[N/2-k]  = Xe[N/2-k] + Xo[N/2-k]*-Conjugate[w[k]]

	Complex expansion:
	 2*Xe[k]     =  (Re[Z[k]] + Re[Z[N/2-k]]) + I*(Im[Z[k]] - Im[Z[N/2-k]])
	 2*Xo[k]     =  (Im[Z[k]] + Im[Z[N/2-k]]) - I*(Re[Z[k]] - Re[Z[N/2-k]])
	 2*Xe[N/2-k] =  (Re[Z[k]] + Re[Z[N/2-k]]) - I*(Im[Z[k]] - Im[Z[N/2-k]])
	 2*Xo[N/2-k] =  (Im[Z[k]] + Im[Z[N/2-k]]) + I*(Re[Z[k]] - Re[Z[N/2-k]])

	Collecting common terms:
	 s0 = Re[Z[k]] + Re[Z[N/2-k]]
	 d0 = Re[Z[k]] - Re[Z[N/2-k]]
	 s1 = Im[Z[k]] + Im[Z[N/2-k]]
	 d1 = Im[Z[k]] - Im[Z[N/2-k]]
	 2*Xe[k]     = s0 + I*d1
	 2*Xo[k]     = s1 - I*d0
	 2*Xe[N/2-k] = s0 - I*d1
	 2*Xo[N/2-k] = s1 + I*d0

	Complex expansion of twiddle factors and collection of common terms:
	 a0 =  c*s1 + s*d0
	 a1 = -s*s1 + c*d0
	 2*Xo[k]*w                    =  (c*s1 + s*d0) - I*(-s*s1 + c*d0) =  a0 - I*a1
	 2*Xo[N/2-k]*-Conjugate[w[k]] = -(c*s1 + s*d0) - I*(-s*s1 + c*d0) = -a0 - I*a1

	Finally, for the case of k==N/4, we have:
	 w = E^(-I*2Pi*(N/4) / N) = E^(-I*Pi/2) = -I
	 Xe[k] =    (Z[k] + Z*[k])/2 = Re[Z[k]] + I*0
	 Xo[k] = -I*(Z[k] - Z*[k])/2 = 0 + I*Im[Z[k]]
	 X[k]  = Xe[k] + Xo[k]*w = Re[Z[k]] + Im[Z[k]] = Z[k]
	And so no work needs to be done for this term.

	*/
	{
		// Note that our oscillators have amplitude 0.5. This is because
		// we can then use a FMA variant in the final calculation of the
		// (a+b)/2 terms.
		// Also note that the twiddles run in conjugated form, because our
		// oscillator is calculating E^(I*2Pi*k/N), whereas we want to use
		// E^(-I*2Pi*k/N).
		float *BufLo = Buf;
		float *BufHi = Buf + N; // Buf + (N/2)*2
		float a0, a1, b0, b1;
		float s0, s1, d0, d1;
		float wc = cosf((float)(2.0*M_PI) / N);
		float ws = sinf((float)(2.0*M_PI) / N);
		float c  = 0.5f * wc; // We start one step ahead, at half amplitude
		float s  = 0.5f * ws;
		a0 = BufLo[0]; // DC and Nyquist are handled separately
		a1 = BufLo[1];
		*BufLo++ = a0 + a1;
		*BufLo++ = a0 - a1;
		for(i=1;i<N/4;i++) {
			BufHi -= 2;
			a0 = BufLo[0]; // Z[k] -> a0,a1
			a1 = BufLo[1];
			b0 = BufHi[0]; // Z[N/2-k] -> b0,b1
			b1 = BufHi[1];

			s0 = a0 + b0;
			s1 = a1 + b1;
			d0 = a0 - b0;
			d1 = a1 - b1;

			a0 = c*s1 - s*d0; // Xo[k]*w -> a0,a1
			a1 = s*s1 + c*d0; // <- a1 needs negation

			BufLo[0] =  0.5f*s0 + a0; // X[k] = Xe[k] + Xo[k]*w
			BufLo[1] =  0.5f*d1 - a1; // <- Undo negation of a1
			BufHi[0] =  0.5f*s0 - a0; // X[N/2-k] = Xe[N/2-k] + Xo[N/2-k]*-w
			BufHi[1] = -0.5f*d1 - a1; // <- Undo negation of a1

			a0 = c;
			b0 = s;
			c = wc*a0 - ws*b0;
			s = ws*a0 + wc*b0;
			BufLo += 2;
		}
	}
}

/************************************************/
// EOF
/************************************************/
