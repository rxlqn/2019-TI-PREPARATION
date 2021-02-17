%如果不满足360倍fs的话，相位信息会有固定偏差
f = 512;
w = 2 * pi * f;
phi = 53.2/360*2*pi;
A = 1;
B = 2;

N = 4096;

t =  0:N-1;
fs = 40960;

signal1  = A*cos(w*t/fs);
signal2  = B*cos(w*t/fs+phi) +2;
plot(signal1)
plot(abs(fft(signal2)));
 