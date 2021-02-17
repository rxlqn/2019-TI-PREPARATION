f1 = 8000;
f2 = 10;
w1 = 2 * pi * f1;
w2 = 2 * pi * f2;
phi = 45/360*2*pi;
A = 128;
B = 2;

N = 350;

t =  1:N;
fs = 800000;

signal1  = A*cos(w1*t/fs) + 127;
signal2  = B*cos(w2*t/fs+phi) +2;

% plot(t,signal1,t,signal2);
% figure
plot(signal1);