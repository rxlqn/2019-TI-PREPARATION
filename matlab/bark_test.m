bark_7 = [1 1 1 -1 -1 1 -1];

data = [1,1,-1,-1,1,-1,-1,1,1,1,1,-1,-1,1,1,-1,1,1,1,-1,-1,-1,-1,-1,1];
[a,b]=xcorr(data,bark_7);%自相关函数
figure(2);
title('巴克码脉压输出');
d=abs(a);
plot(b,d);%画出自相关函数
[m,n] = max(d);
circshift(data,25-n)