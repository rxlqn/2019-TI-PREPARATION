#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

#define SCK PBout(13)
#define CS PBout(11)
#define BUSY PBin(15)
#define MISO PBin(14)

void ADC1_Init(void);//≥ı ºªØ
double GET_ADC(void);
#endif





