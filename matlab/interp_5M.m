f = 1000;
w = 2 * pi * f;
phi = 48/360*2*pi;
phi2 = 49.537/360*2*pi;

A = 1;
B = 2;

N = 5000;

t =  0:N-1;
fs = 5000000;

signal1  = A*cos(w*t/fs+phi2);
signal2  = B*cos(w*t/fs+phi) +2;

L = 0.001*f;

xx = 0:1/L:N-1/L;     %1000¸öµã
% signal1 = spline(t,signal1,xx);
% signal2 = spline(t,signal2,xx);

signal1 = interp1(t,signal1,xx);    
signal2 = interp1(t,signal2,xx);    
 
plot(signal1(1:5000))
N = 5000;
k = 2;  % (k-1)*fs/N
sum = 0;
for n = 1:N 
    sum = sum + signal1(n)*exp(-j*2*pi*(k-1)*(n-1)/N);
end

sum = sum/N;
2*abs(sum)
atan(imag(sum)/real(sum))/(2*pi)*360