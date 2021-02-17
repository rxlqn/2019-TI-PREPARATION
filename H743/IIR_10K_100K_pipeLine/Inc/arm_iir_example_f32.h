
/* Define to prevent recursive inclusion -------------------------------------*/
//#ifndef __DSP_fir_iir_H
//#define __DSP_fir_iir_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

//void IIR(void);

extern void arm_iir_f32_Lp(float fInputData[], float fOutputData[], uint32_t u32Len);


	 

#ifdef __cplusplus
}
#endif
//#endif /*__ adc_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
