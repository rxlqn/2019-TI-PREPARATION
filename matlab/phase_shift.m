 
C = 30*10^-12;
[w,R] = meshgrid(10000:10000:1000000,5000:10000);
% surf(X,Y,Z)
phi = atan(w.*R*C)/(2*pi)*360;
surf(w,R,phi)