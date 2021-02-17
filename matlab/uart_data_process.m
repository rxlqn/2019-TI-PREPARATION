%只读形式打开txt文件
file_t = fopen('output.txt','r');
% file_t = fopen('dpsk_demod2.txt','r');

%以十进制读取,且读取的数据自动排成一列，排的顺序为：先从第一行左边到第一行右边，然后排第二行
raw2 = fscanf(file_t,'%d');
%关闭文件
fclose(file_t);

plot(raw2)