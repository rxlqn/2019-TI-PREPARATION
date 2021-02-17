f1 = 1500;
f2 = 1000;
w1 = 2 * pi * f1;
w2 = 2 * pi * f2;
phi = 180/360*2*pi;
A = 1;
B = 2;

N = 350;

t =  1:N;
fs = 100000;

noise = B*cos(w2*t/fs);

signal1  = A*cos(w1*t/fs+phi);
signal2  = B*cos(w2*t/fs+phi);
input = signal1 + signal2;
[a,b]=xcorr(input,noise);
d = abs(b);
[m,n] = max(d);
circshift(data,25-n)
circshift(a);
% plot(a);
% plot(t,signal1,t,signal2);
figure
subplot 131
plot(noise);
subplot 132
plot(signal1);
subplot 133
plot(signal2);