% dc = 2040;
% bark_7 = [1 1 1 0 0 1 0];
% 
% % before = raw(1:40)-dc;
% for i = 1:25
% temp = raw(1+40*(i-1):40*i)-dc;
% bit(i) = sum(temp.*before);
% % if bit(i)>0
% %     bit(i) = 0;
% % else
% %     bit(i) = 1;
% % end
% before = temp;
% end
% bit_1 =circshift(bit,3);
% [a,b]=xcorr(bark_7,bark_7);%自相关函数
% figure(2);
% title('巴克码脉压输出');
% d=abs(a);
% plot(b,d);%画出自相关函数
subplot 221
plot(raw)
res(raw>1.4/3.3*65535) = 0;
res(raw<1.4/3.3*65535) = 1;
grad = diff(res);
subplot 222
plot(res);
subplot 223
plot(grad);

for i = 1:1000
   if(grad(i) == 1)
       break
   end
end
% for j = i:1000;
%1  34高电平
%0  反之