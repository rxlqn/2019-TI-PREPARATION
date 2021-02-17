close all;clc;clear all;
N=256;
t=-N+1:2*N-1;
f1=39.8;
f2=51.345;
A1=5;
ph1=30;
s=A1*cos(1*(2*pi*t*f1/N/50-ph1*pi/180));%+A1*cos(1*(2*pi*t*f2/N+ph1*pi/180));
win=hanning(N)';
win1=hanning(N)';
win2=conv(win,win1);
win2=win2/sum(win2);
w=pi*2;
s1=s(1:2*N-1);
y1=s1.*win2;
y1a=y1(N:end) + [0 y1(1:N-1)];
Out1=fft(y1a,N);
a1=abs(Out1);
p1=mod(phase(Out1),2*pi);
s2=s(1+N:3*N-1);
y2=s2.*win2;
y2a=y2(N:end) + [0 y2(1:N-1)];
Out2=fft(y2a,N);
a2=abs(Out2);
p2=mod(phase(Out2),2*pi);
g=mod((p2-p1)/pi/2,1);
h=2*pi*g.*(1-g.*g)./sin(pi*g);
aa1=abs((h.^2).*a2)/2;
rr=round(f1);
disp('频率校正值')
fff=floor(f1)+g(rr+1)
disp('振幅校正值')
aaa=aa1(floor(f1)+1);
disp('初相位校正值')
ppp=p1(rr+1)*180/pi;