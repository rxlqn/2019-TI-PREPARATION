#ifndef ADPCM_H
#define ADPCM_H

#include "main.h"
struct adpcm_state
{
    int valprev;
    int index;
};

extern void adpcm_coder(short *indata, signed char *outdata, int len, struct adpcm_state *state);
extern void adpcm_decoder(signed char *indata, short *outdata, int len, struct adpcm_state *state);
 
 #endif /*ADPCM_H*/
