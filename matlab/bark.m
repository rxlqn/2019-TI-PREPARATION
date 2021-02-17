x=[1 1 1 1 1 -1 -1 1 1 -1 1 -1 1];%13位巴克码

bark_7 = [1 1 1 -1 -1 1 -1];
bark_3 = [1 1 -1];
% bark_7 = [1 1 1 0 0 1 0];
bit = [1 0 0 1 0 1 0 1 1 1 1 1 1 1 1 1 1 ];
bit_stream = [bark_3 bit];
[a,b]=xcorr(bark_7);%自相关函数
figure(2);
title('巴克码脉压输出');
d=abs(a);
plot(b,d);%画出自相关函数
