AC6 编译器重定向 printf时改成
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
file 需要#include "stdio.h"

发送端 AD采样半满全满
接收端 串口DMA接收半满全满，处理数据送DDS半满全满

GPIO 初始化
BDMA  LPTIM初始化


若使用串口中断接收，则不能开启过多的其他中断，否则会导致数据丢失