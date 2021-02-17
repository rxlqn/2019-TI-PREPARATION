/**************************************************************************************************
  Filename:       hal_adf4113.c
  Revised:        $Date: 2013-11-17 $
  Revision:       $Revision:  $
  Description:   This file contains the interface to the ADF4113 frequency synthesizer.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include  <msp430g2452.h>
#include  <string.h>
#include  "hal_types.h"
#include  "hal_adf4113.h"

/*********************************************************************
 * CONSTANTS
 */

// ADF4113 Latch Selection - Decide the Control Bits C2 and C1
#define ADF4113_LATCH_SEL_R       0x00
#define ADF4113_LATCH_SEL_N       0x01
#define ADF4113_LATCH_SEL_FUNC    0x02
#define ADF4113_LATCH_SEL_INIT    0x03

// ADF4113 Prescale value : automatically calculate
#define ADF4113_PRESCALE8         0
#define ADF4113_PRESCALE16        1
#define ADF4113_PRESCALE32        2
#define ADF4113_PRESCALE64        3

// ADF4113 Prescale value for minimum required division ratio
#define ADF4113_PRE8_MIN_N        56
#define ADF4113_PRE16_MIN_N       240
#define ADF4113_PRE32_MIN_N       992
#define ADF4113_PRE64_MIN_N       4032

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
static uint8 ADF4113_configured = 0;
static halRCounter_t halRCounter;
static halNCounter_t halNCounter;
static halFuncLatch_t halFuncLatch;
static halFuncLatch_t halInitLatch;

static uint8 Rlatch_bits[3];
static uint8 Nlatch_bits[3];
static uint8 Funclatch_bits[3];
static uint8 Initlatch_bits[3];

/*********************************************************************
 * FUNCTIONS - Local
 */
uint8 HalSynConfig(void);
uint8 halSynSetFreq(uint16 rf_Freq);
uint8 halSynSetCounter(uint16 R_val, uint16 B_val, uint8 A_val, uint8 P_mode);
uint8 halSynSetRLatch(halRCounter_t* RCounter, uint8* RLatch);
uint8 halSynSetNLatch(halNCounter_t* NCounter, uint8* NLatch);
uint8 halSynSetFuncLatch(halFuncLatch_t* Func, uint8* FuncLatch);
uint8 halSynWriteLatch(uint8* Latch);
void TriWire_WriteByte(uint8 output_data);
void delay_time(uint8 time);

/*********************************************************************
 * FUNCTIONS - API
 */
  
/*********************************************************************
 * @fn      HalSynInit
 * @brief   Initialize Timer Service
 * @param   None
 * @return  None
 */
uint8 HalSynInit (void)
{
    /* Initial Setup for synthesizer */
    halSynSetFunc(ADF4113_SET_RESYNC        , ADF4113_RESYNC_NORMAL1    );
    halSynSetFunc(ADF4113_SET_LD_PREC       , ADF4113_LD_PREC_5CYCLE    );
    halSynSetFunc(ADF4113_SET_TEST          , ADF4113_TEST_MODE_OFF     );
    halSynSetFunc(ADF4113_SET_ABP           , ADF4113_ANTI_WIDTH_3P0NS  );
    halSynSetFunc(ADF4113_SET_CP_GAIN       , ADF4113_CP_GAIN2          );
    halSynSetFunc(ADF4113_SET_POWER_DOWN    , ADF4113_PDOWN_NORMAL      );
    halSynSetFunc(ADF4113_SET_CURRENT       , ADF4113_ICP7              );
    halSynSetFunc(ADF4113_SET_COUNTER_CONT  , ADF4113_TCOUNTER0         );
    halSynSetFunc(ADF4113_SET_FAST_LOCK     , ADF4113_FAST_LOCK_OFF     );
    halSynSetFunc(ADF4113_SET_CP_STATE      , ADF4113_CP_OUT_NORMAL     );
    halSynSetFunc(ADF4113_SET_PFD_SIGN      , ADF4113_PFD_NEGATIVE     );
    halSynSetFunc(ADF4113_SET_MUX           , ADF4113_MUX_DIVR_OUT      );
    halSynSetFunc(ADF4113_SET_COUNTER_RESET , ADF4113_COUNTER_RESET_NORM);
    halSynSetFunc(ADF4113_SET_PRESCALE_VAL  , ADF4113_PRESCALE32        );
    halSynSetFunc(ADF4113_SET_CTL_BITS      , ADF4113_LATCH_SEL_R       );
    halSynSetFunc(ADF4113_SET_CTL_BITS      , ADF4113_LATCH_SEL_N       );
    halSynSetFunc(ADF4113_SET_CTL_BITS      , ADF4113_LATCH_SEL_FUNC    );
    HalSynConfig();
    
    return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      HalSynConfig
 * @brief   Configure the Synthesizer at initial startup
 * @param   None
 * @return  None
 ***************************************************************************************************/
uint8 HalSynConfig(void)
{ 
    if(halSynSetFreq(ADF4113_RF_CENTER_FREQ_MHZ) == HAL_SYN_OK) {
        halInitLatch = halFuncLatch;
        halSynSetFunc(ADF4113_SET_CTL_BITS, ADF4113_LATCH_SEL_INIT);
    }

    halSynSetFuncLatch(&halInitLatch, Initlatch_bits);
    halSynSetRLatch(&halRCounter, Rlatch_bits);
    halSynSetNLatch(&halNCounter, Nlatch_bits);
    halSynWriteLatch(Initlatch_bits);
    halSynWriteLatch(Rlatch_bits);
    halSynWriteLatch(Nlatch_bits);
    ADF4113_configured = TRUE;

    return HAL_SYN_OK;
}
 
/***************************************************************************************************
 * @fn      HalSynStart
 * @brief   Start the Synthesizer at certain frequency
 * @param   rf_Freq: MHz in decimal
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 HalSynStart (uint16 rf_Freq)
{ 
    if (ADF4113_configured) {
        halSynSetFreq(rf_Freq);
        halSynSetFuncLatch(&halFuncLatch, Funclatch_bits);
        halSynSetRLatch(&halRCounter, Rlatch_bits);
        halSynSetNLatch(&halNCounter, Nlatch_bits);
        halSynWriteLatch(Funclatch_bits);
        halSynWriteLatch(Rlatch_bits);
        halSynWriteLatch(Nlatch_bits);
    } else {
        return HAL_SYN_NOT_CONFIGURED;
    }

    return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      halSynSetFunc
 * @brief   Set the function value in each latch struct
 * @param   funcSet: the name of function, mode: value of function
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynSetFunc(uint8 funcSet, uint8 mode)
{
    switch(funcSet) {
        case ADF4113_SET_RESYNC:
            halRCounter.dly = (mode & 0x02);
            halRCounter.sync = (mode & 0x01);
            break;
            
        case ADF4113_SET_LD_PREC:
            halRCounter.ldprec = (mode & 0x01);
            break;
            
        case ADF4113_SET_TEST:
            halRCounter.testmode = (mode & 0x03);
            break;
            
        case ADF4113_SET_ABP:
            halRCounter.abp = (mode & 0x03);
            break;
            
        case ADF4113_SET_CP_GAIN:
            halNCounter.cpgain = (mode & 0x01);
            break;
            
        case ADF4113_SET_POWER_DOWN:
            halFuncLatch.pdown1 = (mode & 0x01);
            halFuncLatch.pdown2 = (mode & 0x02);
            break;
        
        case ADF4113_SET_CURRENT:
            halFuncLatch.current_set = ((mode & 0x06) << 3) + (mode & 0x06);
            break;
            
        case ADF4113_SET_COUNTER_CONT:
            halFuncLatch.counter_control = (mode & 0x0F);
            break;
            
        case ADF4113_SET_FAST_LOCK:
            halFuncLatch.flock_en = (mode & 0x02);
            halFuncLatch.flock_mode = (mode & 0x01);
            break;
            
        case ADF4113_SET_CP_STATE:
            halFuncLatch.cp_out_state = (mode & 0x01);
            break;
            
        case ADF4113_SET_PFD_SIGN:
            halFuncLatch.pfd_sign = (mode & 0x01);
            break;
            
        case ADF4113_SET_MUX:
            halFuncLatch.mux_control = (mode & 0x07);
            break;
            
        case ADF4113_SET_COUNTER_RESET:
            halFuncLatch.counter_rst = (mode & 0x01);
            break;
            
        case ADF4113_SET_PRESCALE_VAL:
            halFuncLatch.prescale_val = (mode & 0x03);
            break;
            
        case ADF4113_SET_CTL_BITS:
            if ((mode&0x03) == ADF4113_LATCH_SEL_R) halRCounter.control = ADF4113_LATCH_SEL_R;
            if ((mode&0x03) == ADF4113_LATCH_SEL_N) halNCounter.control = ADF4113_LATCH_SEL_N;
            if ((mode&0x03) == ADF4113_LATCH_SEL_FUNC) halFuncLatch.control = ADF4113_LATCH_SEL_FUNC;
            if ((mode&0x03) == ADF4113_LATCH_SEL_INIT) halInitLatch.control = ADF4113_LATCH_SEL_INIT;
            break;
        
        default:
            return HAL_SYN_INVALID_FUNC;
            break;
    }
    return HAL_SYN_OK;
}

/*********************************************************************
 * FUNCTIONS - Local
 */
 
/***************************************************************************************************
 * @fn      halSynSetFreq
 * @brief   Calculate and Set the counter value in the latches
 * @param   rf_Freq: MHz in decimal
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynSetFreq(uint16 rf_Freq)
{
    uint16  R, B;
    uint8   A, P, p_mode;
    uint16  N_val = 0;
    N_val = (rf_Freq / ADF4113_RES_FREQ_MHZ);
    R = ADF4113_REF_FREQ_MHZ / ADF4113_RES_FREQ_MHZ;
    
    if( N_val < ADF4113_PRE8_MIN_N) { 
        return HAL_SYN_INVALID_PRESCALE; 
    } else if(( N_val> ADF4113_PRE8_MIN_N) && (N_val < ADF4113_PRE16_MIN_N)) { 
        P = 8;  
        p_mode = ADF4113_PRESCALE8;
    } else if(( N_val > ADF4113_PRE16_MIN_N) && (N_val < ADF4113_PRE32_MIN_N)) { 
        P = 16;
        p_mode = ADF4113_PRESCALE16;
        
    } else if((N_val > ADF4113_PRE32_MIN_N) && ( N_val < ADF4113_PRE64_MIN_N)) { 
        P = 32;
        p_mode = ADF4113_PRESCALE32;
        
    } else if( N_val > ADF4113_PRE64_MIN_N) { 
        P = 64; 
        p_mode = ADF4113_PRESCALE64;
    }
    
    A = N_val % P;
    B = (N_val - A) / P;
    if(B < A) return HAL_SYN_INVALID_DIV_N;
    
    halSynSetCounter(R, B, A, p_mode);
    
    return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      halSynSetRLatch
 * @brief   Set the transmit buffer to R Counter
 * @param   RCounter: pointer to halRCounter_t struct, RLatch: pointer to a 3-bytes array (TX buffer)
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynSetRLatch(halRCounter_t* RCounter, uint8* RLatch)
{
    uint32 temp = 0;
    temp = (temp << 1);
    temp = (temp << 1) | RCounter->dly;
    temp = (temp << 1) | RCounter->sync;
    temp = (temp << 1) | RCounter->ldprec;
    temp = (temp << 2) | RCounter->testmode;
    temp = (temp << 2) | RCounter->abp;
    temp = (temp << 14) | RCounter->r_val;
    temp = (temp << 2) | RCounter->control;
    *RLatch = (uint8) (temp & 0x000000FF);
    *(RLatch + 1) = (uint8) ((temp >> 8) & 0x000000FF);
    *(RLatch + 2) = (uint8) ((temp >> 16) & 0x000000FF);

    return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      halSynSetNLatch
 * @brief   Set the transmit buffer to N Counter
 * @param   NCounter: pointer to halNCounter_t struct, NLatch: pointer to a 3-bytes array (TX buffer)
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynSetNLatch(halNCounter_t* NCounter, uint8* NLatch)
{
    uint32 temp = 0;
    temp = (temp << 2);
    temp = (temp << 1) | NCounter->cpgain;
    temp = (temp << 13) | NCounter->b_val;
    temp = (temp << 6) | NCounter->a_val;
    temp = (temp << 2) | NCounter->control;
    *NLatch = (uint8) (temp & 0x000000FF);
    *(NLatch + 1) = (uint8) ((temp >> 8) & 0x000000FF);
    *(NLatch + 2) = (uint8) ((temp >> 16) & 0x000000FF);

    return HAL_SYN_OK;
} 

/***************************************************************************************************
 * @fn      halSynSetFuncLatch
 * @brief   Set the function of the synthesizer
 * @param   Func: pointer to the halFuncLatch_t struct
 *          FuncLatch: pointer to a 3-bytes array (transmitting buffer)
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynSetFuncLatch(halFuncLatch_t* Func, uint8* FuncLatch)
{
    uint32 temp = 0;
    temp = (temp | Func->prescale_val);
    temp = (temp << 1) | Func->pdown2;
    temp = (temp << 6) | Func->current_set;
    temp = (temp << 4) | Func->counter_control;
    temp = (temp << 1) | Func->flock_mode;
    temp = (temp << 1) | Func->flock_en;
    temp = (temp << 1) | Func->cp_out_state;
    temp = (temp << 1) | Func->pfd_sign;
    temp = (temp << 3) | Func->mux_control;
    temp = (temp << 1) | Func->pdown1;
    temp = (temp << 1) | Func->counter_rst;
    temp = (temp << 2) | Func->control;
    *FuncLatch = (uint8) (temp & 0x000000FF);
    *(FuncLatch + 1) = (uint8) ((temp >> 8) & 0x000000FF);
    *(FuncLatch + 2) = (uint8) ((temp >> 16) & 0x000000FF);

    return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      halSynSetCounter
 * @brief   Set the counter value in each latch struct
 * @param   R_val: R counter value, B_val: B counter value, A_val: A counter value
 *          P_mode: Prescaler selection
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynSetCounter(uint16 R_val, uint16 B_val, uint8 A_val, uint8 P_mode)
{
  halRCounter.r_val = R_val;  //(R_val & 0x3FFF);  // 14-bits
  halNCounter.b_val = B_val;  //(B_val & 0x1FFF);  // 13-bits
  halNCounter.a_val = A_val;  //(A_val & 0x3F);    // 6-bits
  halFuncLatch.prescale_val = (P_mode & 0x03);     // 2-bits

  return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      halSynWriteLatch
 * @brief   Write 24-bits data to the latch via 3-wire interface
 * @param   Latch: the TX buffer
 * @return  Status - OK or Not OK
 ***************************************************************************************************/
uint8 halSynWriteLatch(uint8 *Latch)
{
  TriWire_Port &= ~TriWire_LE;
  TriWire_WriteByte(*(Latch + 2));
  TriWire_WriteByte(*(Latch + 1));
  TriWire_WriteByte(*(Latch));
  TriWire_Port |= TriWire_LE;

  return HAL_SYN_OK;
}

/***************************************************************************************************
 * @fn      TriWire_WriteByte
 * @brief   Write 1-byte data to the bus
 * @param   output_data: The data byte to write on the bus
 * @return  None
 ***************************************************************************************************/
void TriWire_WriteByte(uint8 output_data)
{
  uint8 index;

  for(index = 0; index < 8; index++) {
    TriWire_Port &= ~TriWire_SCLK;
    if(output_data & 0x80)
      TriWire_Port |= TriWire_IO;
    else
      TriWire_Port &= ~TriWire_IO;
   
    delay_time(TriWireDELAY);
    TriWire_Port |= TriWire_SCLK;
    delay_time(TriWireDELAY);
    
    output_data = output_data << 1;
  }
}

/***************************************************************************************************
 * @fn      delay_time
 * @brief   simple delay with a NOP loop
 * @param   time = 0 ~ 255, the delay time be be around few tens to hundred of us.
 * @return  None
 ***************************************************************************************************/
void delay_time(uint8 time)
{
  uint8 i;
  for(i = 0; i < time; i++) { ; }
}

/***************************************************************************************************
 * @fn      delayms
 * @brief   delay with ms
 * @param   time = 0 ~ 255, the maximum delay is 255 ms
 * @return  None
 ***************************************************************************************************/
void delayms(uint8 time)  
{
  uint8 n;
  while(time > 0)
  {
    n = 162;
    while(n > 0) n--;
    time--;
  }
}

/***************************************************************************************************
***************************************************************************************************/
