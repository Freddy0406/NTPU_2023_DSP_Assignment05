clc;
clear;
close all;

[x,Fs] = audioread('test.wav');
h_coef = importdata('h.txt');
x_l = x(:,1);
x_r = x(:,2);

count = 1;
frame_size = 441;
total_frame = length(x_l)/frame_size;
H_coef = fft(h_coef).*80;

true_output_zl = zeros(1,80*800);
true_output_zr = zeros(1,80*800);

while(count~=total_frame)
    temp_xl = x_l((1+(count-1)*441):441*count);
    temp_xr = x_r((1+(count-1)*441):441*count);
    zp_xl = upsample(temp_xl,80);
    zp_xr = upsample(temp_xr,80);

    temp_zp_xl = zeros(1,2048);
    temp_zp_xr = zeros(1,2048);
    previous_xl = zeros(1,1024);
    previous_xr = zeros(1,1024);


    for i = 1:80
        temp_zp_xl(1:441) = zp_xl((1+(i-1)*441):441*i);
        temp_zp_xr(1:441) = zp_xr((1+(i-1)*441):441*i);
        temp_XL = fft(temp_zp_xl);
        temp_XR = fft(temp_zp_xr);
        temp_after_XL = temp_XL.*H_coef;
        temp_after_XR = temp_XR.*H_coef;
        after_xl = ifft(temp_after_XL);     %2048
        after_xr = ifft(temp_after_XR);     %2048    

        %% Overlap & Add
        trim_after_xl = after_xl(1:1465);   % trim 2048 into (441+1025-1)
        trim_after_xr = after_xr(1:1465);


        trim_after_xl(1:1024)  = trim_after_xl(1:1024)+previous_xl;
        trim_after_xr(1:1024)  = trim_after_xr(1:1024)+previous_xr;

        previous_xl = trim_after_xl(442:1465);
        previous_xr = trim_after_xr(442:1465);
            
        output_xl((1+(i-1)*441):441*i) = after_xl(1:441);
        output_xr((1+(i-1)*441):441*i) = after_xl(1:441);
    end

    true_output_zl((1+(count-1)*80):80*count) = downsample(output_xl,441);
    true_output_zr((1+(count-1)*80):80*count) = downsample(output_xr,441);
    count = count+1;
end

final_wav = cat(2,true_output_zl',true_output_zr');
audiowrite('output_test.wav',final_wav,8000);






