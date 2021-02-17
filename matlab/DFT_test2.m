%如果不满足360倍fs的话，相位信息会有固定偏差
f = 100;
w = 2 * pi * f;
phi = 53.2/360*2*pi;
A = 1;
B = 2;

N = 1000;

t =  0:N-1;
fs = 100000;

signal1  = A*cos(w*t/fs);
signal2  = B*cos(w*t/fs+phi) +2;
plot(signal1)
plot(abs(fft(signal1)));
k = 21;  % (k-1)*fs/N
k = f*N/fs+1;
sum = 0;
for n = 1:N
    sum = sum + signal2(n)*exp(-j*2*pi*(k-1)*(n-1)/N);
end

sum = sum/N;
2*abs(sum)
atan(imag(sum)/real(sum))/(2*pi)*360