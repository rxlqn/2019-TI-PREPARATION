%插值能够保留正弦的相位和幅度信息，且比直接采样更方便的是可以控制升采样的倍数使得K值不变
%k = f*N/fs+1;

f = 100000;
w = 2 * pi * f;
phi = 45.4/360*2*pi;
A = 1;
B = 2;

N = 10000;

t =  0:N;
fs = 1000000;

signal1  = A*cos(w*t/fs);
signal2  = B*cos(w*t/fs+phi) +2;

L = 0.0001*f;

xx = 0:1/L:N-1/L;     %1000个点
% signal1 = spline(t,signal1,xx);
% signal2 = spline(t,signal2,xx);

signal1 = interp1(t,signal1,xx);    
signal2 = interp1(t,signal2,xx);    
 
plot(signal1)

plot(abs(fft(signal1(1:100))));

N = 100;
k = 2;  % (k-1)*fs/N
sum = 0;
for n = 1:N
    sum = sum + signal2(n)*exp(-j*2*pi*(k-1)*(n-1)/N);
end

sum = sum/N;
2*abs(sum)
atan(imag(sum)/real(sum))/(2*pi)*360