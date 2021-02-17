close all
%只读形式打开txt文件
file_t = fopen('noise.txt','r');
% file_t = fopen('dpsk_demod2.txt','r');

%以十进制读取,且读取的数据自动排成一列，排的顺序为：先从第一行左边到第一行右边，然后排第二行
input = fscanf(file_t,'%f');

%只读形式打开txt文件
file_t = fopen('output.txt','r');
% file_t = fopen('dpsk_demod2.txt','r');

%以十进制读取,且读取的数据自动排成一列，排的顺序为：先从第一行左边到第一行右边，然后排第二行
out = fscanf(file_t,'%f');

%只读形式打开txt文件
file_t = fopen('LMS_input.txt','r');
% file_t = fopen('dpsk_demod2.txt','r');

%以十进制读取,且读取的数据自动排成一列，排的顺序为：先从第一行左边到第一行右边，然后排第二行
ref = fscanf(file_t,'%f');
%关闭文件
fclose(file_t);

%只读形式打开txt文件
file_t = fopen('error_output.txt','r');
% file_t = fopen('dpsk_demod2.txt','r');

%以十进制读取,且读取的数据自动排成一列，排的顺序为：先从第一行左边到第一行右边，然后排第二行
error = fscanf(file_t,'%f');
%关闭文件
fclose(file_t);

plot(ref);
figure

plot(input);
figure
plot(error);
figure
plot(out);