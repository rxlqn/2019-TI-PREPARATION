#include "interpolation.h"
#include "math.h"

/**
 *
 * @param src caller supplied array with data
 * @param src_len len of src
 * @param steps to interpolate
 * @param dst output param will be filled with (src_len - 1) * steps + 1 samples
 */
//void linearInterpolation(float* src, int src_len, int steps, float* dst)
//{
//    float step, y0, dy;
//    float *src_end;
////    if (src_len > 0) {
////        step = 1.0 / steps;
////        for (src_end = src+src_len; *dst++ = y0 = *src++, src < src_end; ) {
////            dy = (*src - y0) * step;
////            for (int i=steps; i>0; i--) {
////                *dst++ = y0 += dy;
////            }
////        }
////    }
////	for(int i = 0;i<src_len;i++)
////	{
////		dst[i*steps] = src[i];
////		for(int j = 1;j<steps;j++)
////		{
////			dst[i*steps+j] = dst[i*steps] + ()
////		}
////	}
//}


void Interpolation(int* src,int* dst, int iSrcLength, int iDstLength)
{
	float k;
	float b;
	float step = 1.0 * (float)iDstLength / (float)iSrcLength;
	int i;
	int j;
	int idx;
	for(i = 0; i < iSrcLength; i++)
	{
		if (i == 0)
		{
			k = src[0] / step;
			b = 0;
		}
		else
		{
			k = (float)(src[i] - src[i-1])/ step;
			b = (float)src[i] - k * (i+1) * step;
		}
		//ÏßÐÔ²åÖµ
		for (j = ceil(i*step); j < ceil((i+1)*step); j++)
		{
			idx = j;
			if(j == 2000)
				return;
			dst[idx] = k * (float)idx + b;
		}
		
	}
 } 
 
