# adf4113_driver
This is the driver implemented on MSP430 for controlling the ADF4113 frequency synthesizer IC.
You can also port this driver to other MCUs. Let's take the 8051 as an example, there are just few steps to do:

step1: Re-assign the GPIOs in hal_adf4113.h
 
    #define TriWire_Port  P1_bit.P1_0 
    #define TriWire_IO    P1_bit.P1_1 
    #define TriWire_SCLK  P1_bit.P1_2 
    #define TriWire_LE    P1_bit.P1_3 

step2: Change the MCU header file in hal_adf4113.c
 
    #include  <msp430g2452.h> -> #include  <ioAT89C52.h>

step3: Change the MCU header file & re-assign the GPIOs used by the buttons in main.c
 
    #include  <msp430g2452.h> -> #include  <ioAT89C52.h> 
    #define BTN1 BV(3) -> #define BTN1 P3_bit.P3_2 
    Modifiy the code to make it run! 

APIs

1. HalSynInit(void) 
 
    Initialize the synthesizer when powered up.
    
2. HalSynStart(uint16 rf_Freq) 
 
    Call this function to start the synthesizer with rf_Freq.
    The unit of the rf_Freq is MHz.
    For exmaple: HalSynStart(2430) will tell the synthesizer 
                 to output a 2.43 GHz sinudoidal signal.

3. halSynSetFunc(uint8 funcSet, uint8 mode)
 
    funcSet: (Please see the datasheet for the detail)
    
            ADF4113_SET_RESYNC
            ADF4113_SET_LD_PREC      
            ADF4113_SET_TEST         
            ADF4113_SET_ABP          
            ADF4113_SET_CP_GAIN      
            ADF4113_SET_POWER_DOWN   
            ADF4113_SET_CURRENT      
            ADF4113_SET_COUNTER_CONT 
            ADF4113_SET_FAST_LOCK    
            ADF4113_SET_CP_STATE     
            ADF4113_SET_PFD_SIGN     
            ADF4113_SET_MUX          
            ADF4113_SET_COUNTER_RESET
            ADF4113_SET_CTL_BITS     
            ADF4113_SET_PRESCALE_VAL 
 
    mode: (Please see hal_adf4113.h and the datasheet for the detail)
    
            ADF4113_RESYNC_NORMAL1   
            ADF4113_RESYNC_NONDELAYRF
            ADF4113_RESYNC_NORMAL2   
            ADF4113_RESYNC_DELAYRF   
            ADF4113_LD_PREC_3CYCLE   
            ADF4113_LD_PREC_5CYCLE   
            ADF4113_TEST_MODE_OFF    
            ADF4113_ANTI_WIDTH_3P0NS 
            ADF4113_ANTI_WIDTH_1P5NS 
            ADF4113_ANTI_WIDTH_6P0NS 
            ...

Note:
    Everytime when you change the settings, please remember to call HalSynStart()
to apply new changes. For example:
 
    halSynSetFunc(ADF4113_SET_LD_PREC, ADF4113_LD_PREC_5CYCLE); 
    halSynSetFunc(ADF4113_SET_ABP, ADF4113_ANTI_WIDTH_1P5NS); 
    halSynSetFunc(ADF4113_SET_CURRENT, ADF4113_ICP6); 
    halSynSetFunc(ADF4113_SET_COUNTER_CONT, ADF4113_TCOUNTER10); 
    halSynSetFunc(ADF4113_SET_MUX, ADF4113_MUX_DIVN_OUT); 
    HalSynStart(2423); 
    