AC6 编译器重定向 printf时改成
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
file 需要#include "stdio.h"
