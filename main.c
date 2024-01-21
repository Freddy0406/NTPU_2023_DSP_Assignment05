# include "function.h"

int main(){

    FILE *fp = fopen("test.wav", "rb");                                    //讀入音檔
    FILE *fp_out = fopen("output.wav", "wb");                               //開啟輸出檔案
    struct WaveHeader Wavin_head;                                           //儲存讀入wave之標頭
    struct WaveHeader Wavout_head;                                          //儲存輸出wave之標頭
    fread(&Wavin_head,sizeof(Wavin_head),1,fp);                             //讀入wave標頭
    int wav_time = Wavin_head.DATALen/Wavin_head.bytepsec;                  //計算音檔長度(單位：秒)

    short *data_read = (short*)malloc(sizeof(short)*(data_L));              //讀取wav雙聲道        
    cplx data_zp_L [zp_N];                 //zero padding 左聲道
    cplx data_zp_R [zp_N];                 //zero padding 右聲道

    int n,i;

    /*Create LPF coefficient*/
    FILE *fptxt  = fopen("h.txt","w+");                                     //開啟txt檔以寫入LPF coeficient
    cplx h [FFT_N];                             //儲存LPF
    cplx H [FFT_N] = {0};                             //儲存fft後的LPF
    /*Make LPF*/
    for(n=0;n<FFT_N;n++) {
        if(n<P){
            h[n] = L*low_pass(MOrder, n);
        }
            
        else{
            h[n] = 0.0;
        }
        fprintf(fptxt,"%lf\n",h[n]);                                        //write into txt
        fflush(fptxt); 
    }
    fclose(fptxt);

    fft(h,H,FFT_N);

    // for(i=0;i<FFT_N;i++){
    //     H[i] = H[i]*80.0; 
    // }

    // for (int i = 0; i < FFT_N; i++) {
    //     printf("(%f, %f)\n", creal(H[i]), cimag(H[i]));
    // }

    /*Make output header*/
    Wavout_head = Wavin_head;
    Wavout_head.samplehz = FS_out;                                  //寫入欲輸出之取樣率
	Wavout_head.bytepsec = FS_out*Wavout_head.channels*(Wavout_head.sample_bits/8); //計算每秒有多少byte的資料
	Wavout_head.DATALen = Wavout_head.bytepsec*wav_time;            //計算資料總大小
	Wavout_head.total_Len = 36+Wavout_head.DATALen;                 //計算整個檔案大小(36+檔案大小)
    fwrite(&Wavout_head,sizeof(Wavout_head),1,fp_out);              //Write output wave header


    short *out_L = (short*)calloc(L,sizeof(short));                 //output 左聲道
    short *out_R = (short*)calloc(L,sizeof(short));                 //output 右聲道

    short prev_L[P-1] = {0};        //overlap and add
    short prev_R[P-1] = {0};        //overlap and add

    cplx temp_zp_xl [FFT_N] = {0.0};
    cplx temp_zp_xr [FFT_N] = {0.0};
    cplx temp_XL [FFT_N] = {0.0};
    cplx temp_XR [FFT_N] = {0.0};
    cplx H_L [FFT_N];                       //經過lpf 左聲道
    cplx H_R [FFT_N];                       //經過lpf 右聲道
    cplx h_L [FFT_N] = {0.0};                       //經過lpf ifft 左聲道
    cplx h_R [FFT_N] = {0.0};                       //經過lpf ifft 右聲道
    short trim_xl [trim_length] = {0};
    short trim_xr [trim_length] = {0};
    short ready_out_L [441*80];
    short ready_out_R [441*80];



    int m = 0;
    while( fread(data_read, sizeof(short), data_L, fp) ) {
        /*convert data type & zero padding*/
        int count = 0;
        for(i=0;i<(data_L);i+=2) {
            data_zp_L[(i*L)/2] = (float) data_read[i];
            data_zp_R[(i*L)/2] = (float) data_read[i+1];
        }

        for(int b = 0;b<(P-1);b++){
            prev_L[b] = 0;
            prev_R[b] = 0;
        }


        for(int a = 0;a<80;a++){

            for(i = 0;i<441;i++){
                temp_zp_xl[i] = data_zp_L[i+441*a];             //抓出441筆資料
                temp_zp_xr[i] = data_zp_R[i+441*a];             //抓出441筆資料
            }

            fft(temp_zp_xl,temp_XL,FFT_N);
            fft(temp_zp_xr,temp_XR,FFT_N);


            //頻域相乘 = 時域convolution
            for(i = 0;i<FFT_N;i++){
                H_L[i] = temp_XL[i]*H[i];               //2048 points
                H_R[i] = temp_XR[i]*H[i];               //2048 points
            }

            ifft(H_L,h_L,FFT_N);
            ifft(H_R,h_R,FFT_N);

            for(i = 0;i<trim_length; i++){
                trim_xl[i] = (short)creal(h_L[i]);
                trim_xr[i] = (short)creal(h_R[i]);
            }

            //Add previous
            for(i = 0;i<1024;i++){
                trim_xl[i] = trim_xl[i]+prev_L[i];
                trim_xr[i] = trim_xr[i]+prev_R[i];
            }

            for(i = 441; i<trim_length;i++){
                prev_L[i-441] = trim_xl[i];
                prev_R[i-441] = trim_xr[i];
            }
            for(i = 0;i<441;i++){
                ready_out_L[i+a*441] =  trim_xl[i];
                ready_out_R[i+a*441] =  trim_xr[i];
            }
        }

        for(i=0;i<(441*80);i++){
            if((i%M)==0){
                fwrite(ready_out_L+i, sizeof(short), 1, fp_out);
                fwrite(ready_out_L+i, sizeof(short), 1, fp_out);
            }
        }



        m++;
    }

    free(data_read);
    free(out_L);
    free(out_R);
    fclose(fp);
    fclose(fp_out);

    return 0;
}