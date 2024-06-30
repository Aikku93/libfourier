/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

#include "Fourier_iFFTRe_Template.h"
void Fourier_iFFTRe(float *Buf, float *Tmp, int N) {
	Fourier_iFFTRe_Template(Buf, Tmp, N);
}

/************************************************/
// EOF
/************************************************/
