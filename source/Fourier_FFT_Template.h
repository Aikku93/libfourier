/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

FOURIER_FORCE_INLINE
void Fourier_FFT_Template(float *Buf, float *Tmp, int N) {
	int i;
	FOURIER_ASSUME_ALIGNED(Buf, FOURIER_ALIGNMENT);
	FOURIER_ASSUME_ALIGNED(Tmp, FOURIER_ALIGNMENT);
	FOURIER_ASSUME(N >= 4);

	// Stop condition
	if(N == 4) {
		Fourier_FFT_4(Buf);
		return;
	}

	// Split
	// Note that we're spltting complex values here
	{
		const float *Src  = Buf;
		      float *DstE = Tmp;
		      float *DstO = Tmp + N; // Tmp + (N/2)*2
#if FOURIER_VSTRIDE > 1
		Fourier_Vec_t a, b, t0, t1;
		for(i=0;i<N/2;i+=FOURIER_VSTRIDE/2) {
			a = FOURIER_VLOAD(Src); Src += FOURIER_VSTRIDE;
			b = FOURIER_VLOAD(Src); Src += FOURIER_VSTRIDE;
			FOURIER_VSPLITCPLX_EVEN_ODD(a, b, &t0, &t1);
			FOURIER_VSTORE(DstE, t0); DstE += FOURIER_VSTRIDE;
			FOURIER_VSTORE(DstO, t1); DstO += FOURIER_VSTRIDE;
		}
#else
		for(i=0;i<N/2;i++) {
			*DstE++ = *Src++;
			*DstE++ = *Src++;
			*DstO++ = *Src++;
			*DstO++ = *Src++;
		}
#endif
	}

	// Recurse
	Fourier_FFT(Tmp,   Buf,   N/2);
	Fourier_FFT(Tmp+N, Buf+N, N/2);

	// Combine
	{
		const float *SrcE  = Tmp;
		const float *SrcO  = Tmp + N;
		      float *DstLo = Buf;
		      float *DstHi = Buf + N;
#if FOURIER_VSTRIDE > 1
		Fourier_Vec_t a, b, qr, qi;
		Fourier_Vec_t c, s, t0, t1;
		if(FOURIER_VSTRIDE < 8 || N > 8) {
			t1 = FOURIER_VMUL(FOURIER_VSET1(4.0f/N), FOURIER_VSET_LINEAR_RAMP());
			c  = Fourier_Cos(t1);
			s  = Fourier_Sin(t1);
			Fourier_Vec_t wc = Fourier_Cos(FOURIER_VSET1((float)FOURIER_VSTRIDE * 4.0f / N));
			Fourier_Vec_t ws = Fourier_Sin(FOURIER_VSET1((float)FOURIER_VSTRIDE * 4.0f / N));
			for(i=0;i<N/2;i+=FOURIER_VSTRIDE) {
				t0 = FOURIER_VLOAD(SrcO); SrcO += FOURIER_VSTRIDE;
				t1 = FOURIER_VLOAD(SrcO); SrcO += FOURIER_VSTRIDE;
				FOURIER_VSPLIT_EVEN_ODD(t0, t1, &a, &b);
				qr = FOURIER_VMUL(c, a);
				qr = FOURIER_VFMA(s, b, qr);
				qi = FOURIER_VMUL(c, b);
				qi = FOURIER_VNFMA(s, a, qi);
				FOURIER_VINTERLEAVE(qr, qi, &t0, &t1);
				a = FOURIER_VLOAD(SrcE); SrcE += FOURIER_VSTRIDE;
				b = FOURIER_VLOAD(SrcE); SrcE += FOURIER_VSTRIDE;

				qr = FOURIER_VADD(a, t0);
				qi = FOURIER_VADD(b, t1);
				a  = FOURIER_VSUB(a, t0);
				b  = FOURIER_VSUB(b, t1);
				FOURIER_VSTORE(DstLo, qr); DstLo += FOURIER_VSTRIDE;
				FOURIER_VSTORE(DstLo, qi); DstLo += FOURIER_VSTRIDE;
				FOURIER_VSTORE(DstHi, a);  DstHi += FOURIER_VSTRIDE;
				FOURIER_VSTORE(DstHi, b);  DstHi += FOURIER_VSTRIDE;

				a = c;
				b = s;
				c = FOURIER_VMUL(wc, a);
				c = FOURIER_VNFMA(ws, b, c);
				s = FOURIER_VMUL(ws, a);
				s = FOURIER_VFMA(wc, b, s);
			}
		} else {
			// N == 8 is a special case for 8-wide vectors, because the code
			// above was optimized to make full use of the vector registers,
			// whereas the N == 8 case can't do that.
			// To avoid two sets of MUL+FMA, we can expand the odd values
			// directly into their Re,Im pair targets, and multiply that with
			// the appropriate twiddle factors.
			// These "expanded" twiddle factors are:
			//  a = {c0,-s0, c1,-s1, c2,-s2, c3,-s3}
			//  b = {s0, c0, s1, c1, s2, c2, s3, c3}
			// Then:
			//  Even + (a*ReExpand[Odd] + b*ImExpand[Odd])
			// Where ReExpand[x] gives {Re[x0],Re[x0],Re[x1],Re[x1]...},
			// and ImExpand[x] gives {Im[x0],Im[x0],Im[x1],Im[x1]...}.
			static const float tc[] = {
				1.0f,0.0f,+0x1.6A09E6p-1f,-0x1.6A09E6p-1f,0.0f,-1.0f,-0x1.6A09E6p-1f,-0x1.6A09E6p-1f
			};
			static const float ts[] = {
				0.0f,1.0f,+0x1.6A09E6p-1f,+0x1.6A09E6p-1f,1.0f, 0.0f,+0x1.6A09E6p-1f,-0x1.6A09E6p-1f
			};
			c = FOURIER_VLOAD(tc);
			s = FOURIER_VLOAD(ts);

			t0 = FOURIER_VLOAD(SrcO);
			FOURIER_VEXPANDCPLX(t0, &a, &b);
			qr = FOURIER_VMUL(c, a);
			qr = FOURIER_VFMA(s, b, qr);
			qi = FOURIER_VLOAD(SrcE);

			t0 = FOURIER_VADD(qi, qr);
			t1 = FOURIER_VSUB(qi, qr);
			FOURIER_VSTORE(DstLo, t0);
			FOURIER_VSTORE(DstHi, t1);
		}
#else
		float a, b, qr, qi;
		float c  = 1.0f;
		float s  = 0.0f;
		float wc = Fourier_Cos(4.0f / N);
		float ws = Fourier_Sin(4.0f / N);
		for(i=0;i<N/2;i++) {
			a = *SrcO++;
			b = *SrcO++;
			qr =  c*a + s*b;
			qi = -s*a + c*b;
			a = *SrcE++;
			b = *SrcE++;

			*DstLo++ = a + qr;
			*DstLo++ = b + qi;
			*DstHi++ = a - qr;
			*DstHi++ = b - qi;

			a = c;
			b = s;
			c = wc*a - ws*b;
			s = ws*a + wc*b;
		}
#endif
	}
}

/************************************************/
// EOF
/************************************************/
