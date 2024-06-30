/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

#include "Fourier_FFTRe_Template.h"
void Fourier_FFTRe(float *Buf, float *Tmp, int N) {
	Fourier_FFTRe_Template(Buf, Tmp, N);
}

/************************************************/
// EOF
/************************************************/
