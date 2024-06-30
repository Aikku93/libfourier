/************************************************/
#include "Fourier.h"
#include "Fourier_Helper.h"
/************************************************/

FOURIER_FORCE_INLINE
void Fourier_iFFT_Template(float *Buf, float *Tmp, int N) {
	int i;
	FOURIER_ASSUME_ALIGNED(Buf, FOURIER_ALIGNMENT);
	FOURIER_ASSUME_ALIGNED(Tmp, FOURIER_ALIGNMENT);
	FOURIER_ASSUME(N >= 4);

	// Stop condition
	if(N == 4) {
		Fourier_iFFT_4(Buf);
		return;
	}

	// Split
	{
		const float *SrcLo = Buf;
		const float *SrcHi = Buf + N; // Buf + (N/2)*2
		      float *DstE  = Tmp;
		      float *DstO  = Tmp + N;
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
				qr = FOURIER_VLOAD(SrcLo); SrcLo += FOURIER_VSTRIDE;
				qi = FOURIER_VLOAD(SrcLo); SrcLo += FOURIER_VSTRIDE;
				t0 = FOURIER_VLOAD(SrcHi); SrcHi += FOURIER_VSTRIDE;
				t1 = FOURIER_VLOAD(SrcHi); SrcHi += FOURIER_VSTRIDE;
				a  = FOURIER_VADD(qr, t0);
				b  = FOURIER_VADD(qi, t1);
				qr = FOURIER_VSUB(qr, t0);
				qi = FOURIER_VSUB(qi, t1);
				FOURIER_VSTORE(DstE, a); DstE += FOURIER_VSTRIDE;
				FOURIER_VSTORE(DstE, b); DstE += FOURIER_VSTRIDE;

				FOURIER_VSPLIT_EVEN_ODD(qr, qi, &t0, &t1);
				a = FOURIER_VMUL(c, t0);
				a = FOURIER_VNFMA(s, t1, a);
				b = FOURIER_VMUL(s, t0);
				b = FOURIER_VFMA(c, t1, b);
				FOURIER_VINTERLEAVE(a, b, &t0, &t1);
				FOURIER_VSTORE(DstO, t0); DstO += FOURIER_VSTRIDE;
				FOURIER_VSTORE(DstO, t1); DstO += FOURIER_VSTRIDE;

				a = c;
				b = s;
				c = FOURIER_VMUL(wc, a);
				c = FOURIER_VNFMA(ws, b, c);
				s = FOURIER_VMUL(ws, a);
				s = FOURIER_VFMA(wc, b, s);
			}
		} else {
			// Just like FFT, we have to special-case N==8
			static const float tc[] = {
				1.0f,0.0f,+0x1.6A09E6p-1f,+0x1.6A09E6p-1f, 0.0f,1.0f,-0x1.6A09E6p-1f,+0x1.6A09E6p-1f
			};
			static const float ts[] = {
				0.0f,1.0f,-0x1.6A09E6p-1f,+0x1.6A09E6p-1f,-1.0f,0.0f,-0x1.6A09E6p-1f,-0x1.6A09E6p-1f
			};
			c = FOURIER_VLOAD(tc);
			s = FOURIER_VLOAD(ts);

			t0 = FOURIER_VLOAD(SrcLo); SrcLo += FOURIER_VSTRIDE;
			t1 = FOURIER_VLOAD(SrcHi); SrcHi += FOURIER_VSTRIDE;
			qi = FOURIER_VADD(t0, t1);
			qr = FOURIER_VSUB(t0, t1);
			FOURIER_VSTORE(DstE, qi);

			FOURIER_VEXPANDCPLX(qr, &a, &b);
			qr = FOURIER_VMUL(c, a);
			qr = FOURIER_VFMA(s, b, qr);
			FOURIER_VSTORE(DstO, qr);
		}
#else
		float a, b, qr, qi;
		float c  = 1.0f;
		float s  = 0.0f;
		float wc = Fourier_Cos(4.0f / N);
		float ws = Fourier_Sin(4.0f / N);
		for(i=0;i<N/2;i++) {
			qr = *SrcLo++;
			qi = *SrcLo++;
			a  = *SrcHi++;
			b  = *SrcHi++;
			*DstE++ = qr + a;
			*DstE++ = qi + b;
			qr = qr - a;
			qi = qi - b;

			a = c*qr - s*qi;
			b = s*qr + c*qi;
			*DstO++ = a;
			*DstO++ = b;

			a = c;
			b = s;
			c = wc*a - ws*b;
			s = ws*a + wc*b;
		}
#endif
	}

	// Recurse
	Fourier_iFFT(Tmp,   Buf,   N/2);
	Fourier_iFFT(Tmp+N, Buf+N, N/2);

	// Combine
	{
		const float *SrcE = Tmp;
		const float *SrcO = Tmp + N;
		      float *Dst  = Buf;
#if FOURIER_VSTRIDE > 1
		Fourier_Vec_t a, b, t0, t1;
		for(i=0;i<N/2;i+=FOURIER_VSTRIDE/2) {
			t0 = FOURIER_VLOAD(SrcE); SrcE += FOURIER_VSTRIDE;
			t1 = FOURIER_VLOAD(SrcO); SrcO += FOURIER_VSTRIDE;
			FOURIER_VINTERLEAVECPLX(t0, t1, &a, &b);
			FOURIER_VSTORE(Dst, a); Dst += FOURIER_VSTRIDE;
			FOURIER_VSTORE(Dst, b); Dst += FOURIER_VSTRIDE;
		}
#else
		for(i=0;i<N/2;i++) {
			*Dst++ = *SrcE++;
			*Dst++ = *SrcE++;
			*Dst++ = *SrcO++;
			*Dst++ = *SrcO++;
		}
#endif
	}
}

/************************************************/
// EOF
/************************************************/
