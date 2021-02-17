#include  <msp430g2452.h>
#include  "hal_adf4113.h"

#define BTN1 BV(3)
//#define BTN2 P3_bit.P3_3

void main()
{
    WDTCTL = WDTPW + WDTHOLD;
    P2DIR = 0x07;
    P1OUT = BIT3;    // 設定PT1.3為高電位 
    P1REN = P1OUT;   // 設定PT1.3上拉電阻
    // P2OUT = 0x30;
 
    TriWire_Port |= TriWire_LE;

    HalSynInit();
  
    while(1)
    {
        if ((P1IN & BTN1) == 0) {
            delayms(50);
            if ((P1IN & BTN1 ) == 0) {
                while((P1IN & BTN1) == 0);
                HalSynStart(2400);
            }       
        }
        /*
        if (BTN2 == 0) {
            delayms(50);
            if (BTN2 == 0) {
                while(BTN2 == 0);
                halSynSetFunc(ADF4113_SET_LD_PREC, ADF4113_LD_PREC_5CYCLE);
                halSynSetFunc(ADF4113_SET_ABP, ADF4113_ANTI_WIDTH_1P5NS);
                halSynSetFunc(ADF4113_SET_CURRENT, ADF4113_ICP6);
                halSynSetFunc(ADF4113_SET_COUNTER_CONT, ADF4113_TCOUNTER10);
                halSynSetFunc(ADF4113_SET_MUX, ADF4113_MUX_DIVN_OUT);
                HalSynStart(2423);
            }       
        }
        */
    }
}
