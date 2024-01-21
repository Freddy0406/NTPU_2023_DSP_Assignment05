#ifndef __FUNCTION_H__
#define __FUNCTION_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <complex.h>

#define PI 3.141592653589793f
#define data_L 441*2                // 20ms frame size 44100*0.01 = 441 (Stereo = 441 X 2 = 882)
#define FL 50.0f                 	// cutoff frequency = 50 Hz
#define FS 44100.0f                 // sampling frequency
#define FS_out 8000					//Output sampling frequency
#define MOrder 512                 	// LPF Order
#define M 441   					                                           
#define L 80
#define P (2*MOrder+1)
#define zp_N L*(data_L/2)
#define FFT_N 2048
#define trim_length (M+2*MOrder-1)


typedef double complex cplx;

struct WaveHeader 
{
	char chRIFF[4];                 //RIFF tag
	int32_t  total_Len;             //檔案(header+語音)長度 = 36 + dataSize
	char chWAVE[4];                 //WAVE tag
	char chFMT[4];                  //fmt tag
	int32_t  FMTLen;               
	int16_t fmt_pcm;                //格式類別:1(PCM)
	int16_t  channels;              //聲道數
	int32_t samplehz;               //取樣點/秒
	int32_t bytepsec;               //位元速率 = 取樣頻率*位元深度/8
	int16_t sample_size;            //一個取樣多聲道資料塊大小(bytes)
	int16_t sample_bits;            //取樣位元深度(m)
	char chDATA[4];                 //data tag
	int32_t  DATALen;               //語音資料的大小
};



float hamming(int N, int n);
float low_pass(int m, int n);
void fft(cplx buf[], cplx out[], int n);
void ifft(cplx buf[], cplx out[], int n);









#endif