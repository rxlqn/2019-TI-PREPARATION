#include "sound_feature.h"
#include "math.h"

static feature_extraction_struct featext_wksp;


 void zcr(const float* src,float* dst,u16 len)
{
    const int DIFFLEN = len - 1;
    int i = 0;
    int sum = 0;
    for(i = 0; i < DIFFLEN; ++i)
    {
        if(signbit(src[i + 1]) - signbit(src[i]))
        {
            ++sum;
        }
    }
    *dst = (float)sum / DIFFLEN;
}