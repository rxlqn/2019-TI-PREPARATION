#include "LMS.h"
#include "arm_math.h"

void arm_lms_init_f32	(	arm_lms_instance_f32 * 	S,
uint16_t 	numTaps,
float32_t * 	pCoeffs,
float32_t * 	pState,
float32_t 	mu,
uint32_t 	blockSize 
)	