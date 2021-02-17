

#include "bsp.h"			/* µ×²ãÓ²¼þÇý¶¯ */
#include "ringbuf.h"

#ifndef		_DMA_TimerADC_MD_h_
#define		_DMA_TimerADC_MD_h_

extern tRingBufU16Object sADC_RingBuf;
void ADC_Config(uint32_t u32BufferSize);

#endif

