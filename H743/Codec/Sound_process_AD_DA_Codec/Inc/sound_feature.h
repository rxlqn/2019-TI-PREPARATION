#include "main.h"

typedef struct
{
    int fs;                             // sampling rate
    int signalsize;                     // input signal length (long-term)
    float** filterbanks;               // the triangular filterbank for MFCC computation
    int* chroma_nummat;                 // the chroma matrices (first half: nChroma, second half: nFreqsPerChroma[nChroma])
    float* features;                   // features (output)
    float* signal_prev_freq_domain;
    float signal_prev_freq_domain_sum;
} feature_extraction_struct;

 void zcr(const float* src,float* dst,u16 len);
