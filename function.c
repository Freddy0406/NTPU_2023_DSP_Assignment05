# include "function.h"


/* hamming: for n=0,1,2,...N, length of N+1 */
float hamming(int N, int n)
{
	return 0.54 - 0.46 * cosf(2*PI*((float)(n))/((float)N));
}

/* low-pass filter coef: n=0,1,2...,2M */
float low_pass(int m, int n)
{
	float wc = 2*PI*FL/FS;
	if(n==m) {// L'Hopital's Rule
		return wc/PI;
	}
	else {
		return sinf(wc*((float)(n-m)))/PI/((float)(n-m)) * hamming(2*m+1, n);
	}
}



// FFT
void fft(cplx buf[], cplx out[], int n) {


    if (n <= 1) {
        out[0] = buf[0];
    } else {
        cplx even[n / 2];
        cplx odd[n / 2];
        cplx even_out[n / 2];
        cplx odd_out[n / 2];

        for (int i = 0; i < n / 2; i++) {
            even[i] = buf[i * 2];
            odd[i] = buf[i * 2 + 1];
        }

        fft(even, even_out, n / 2);
        fft(odd, odd_out, n / 2);

        for (int i = 0; i < n / 2; i++) {
            double t = -2.0 * M_PI * i / n;
            cplx twiddle = cexp(I * t) * odd_out[i];
            out[i] = even_out[i] + twiddle;
            out[i + n / 2] = even_out[i] - twiddle;
        }
    }
}

// IFFT
void ifft(cplx input[], cplx output[], int n) {

    for (int i = 0; i < n; i++) {
        input[i] = conj(input[i]);
    }
		cplx final[n];
    fft(input, output, n);

	for (int i = 0; i < FFT_N; i++) {
		output[i] = conj(output[i])/n;
	}

}
