# include "function.h"

int main(){

    FILE *fp = fopen("test.wav", "rb");                                    //讀入音檔
    FILE *fp_out = fopen("output.wav", "wb");                               //開啟輸出檔案
    struct WaveHeader Wavin_head;                                           //儲存讀入wave之標頭
    struct WaveHeader Wavout_head;                                          //儲存輸出wave之標頭
    fread(&Wavin_head,sizeof(Wavin_head),1,fp);                             //讀入wave標頭
    int wav_time = Wavin_head.DATALen/Wavin_head.bytepsec;                  //計算音檔長度(單位：秒)

    short *data_read = (short*)malloc(sizeof(short)*(data_L));              //讀取wav雙聲道        
    cplx data_zp_L [FFT_N];                 //zero padding 左聲道
    cplx data_zp_R [FFT_N];                 //zero padding 右聲道
    cplx fft_L [FFT_N];                     //zero padding through fft 左聲道
    cplx fft_R [FFT_N];                     //zero padding through fft 右聲道
    cplx H_L [FFT_N];                       //經過lpf 左聲道
    cplx H_R [FFT_N];                       //經過lpf 右聲道
    cplx h_L [FFT_N] = {0.0};                       //經過lpf ifft 左聲道
    cplx h_R [FFT_N] = {0.0};                       //經過lpf ifft 右聲道
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

    int m = 0;
    while( fread(data_read, sizeof(short), data_L, fp) ) {
        /*convert data type*/
        int count = 0;
        for(i=0;i<(data_L);i+=2) {
            data_zp_L[count] = (float) data_read[i];
            data_zp_R[count] = (float) data_read[i+1];
            count++;
        }
        /*zero padding*/
        for(i = count;i<FFT_N;i++){
            data_zp_L[i] =  0.0;
            data_zp_R[i] =  0.0;
        }

        fft(data_zp_L,fft_L,FFT_N);
        fft(data_zp_R,fft_R,FFT_N);

        for(i = 0;i<FFT_N;i++){
            H_L[i] = fft_L[i]*H[i];
            H_R[i] = fft_R[i]*H[i];
        }

        ifft(H_L,h_L,FFT_N);
        ifft(H_R,h_R,FFT_N);
        if(m==230){
            for(i=0;i<FFT_N;i++){
                    printf("%d\n",i);
                    printf("(%f, %f)\n", creal(h_L[i]), cimag(h_L[i]));
            }
        }

        // /*overlap and add*/
        // overlap_add(lpf_L , out_L , prev_L);
        // overlap_add(lpf_R , out_R , prev_R);


        // for(i=0;i<L;i++) {
        //     fwrite(out_L+i, sizeof(short), 1, fp_out);
        //     fwrite(out_R+i, sizeof(short), 1, fp_out);
        // }
        m++;
    }

    free(data_read);
    free(out_L);
    free(out_R);
    fclose(fp);
    fclose(fp_out);

    return 0;
}