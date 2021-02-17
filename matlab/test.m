zero = zeros(1,50);
sig = [zero signal1];
subplot 121
plot(sig);
sig1 = sig(5:205) - 127;
sig2 = sig(105:305) - 127;
subplot 122

plot(sig1.*sig2);
