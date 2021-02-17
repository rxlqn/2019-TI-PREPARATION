/**************************************************************************************************
  Filename:       hal_adf4113.h
  Revised:        $Date: 2013-11-17 $
  Revision:       $Revision: $
  Description:    This file contains the interface to the ADF4113 frequency synthesizer.
**************************************************************************************************/
#ifndef HAL_ADF4113_H
#define HAL_ADF4113_H

#include  "hal_types.h"

#define BV(n) (1 << n)

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/
#define TriWire_Port  P2OUT
#define TriWire_IO    BV(0)
#define TriWire_SCLK  BV(1)
#define TriWire_LE    BV(2)
#define TriWireDELAY  1

// Frequency Settings
// Initally, the synthesizer will operate at 2450 MHz
#define ADF4113_REF_FREQ_MHZ          10
#define ADF4113_RF_CENTER_FREQ_MHZ    2450
#define ADF4113_RES_FREQ_MHZ          1

// Function Set Name (funSet)
// halSynSetFunc(uint8 funcSet, uint8 mode);
#define ADF4113_SET_RESYNC        0
#define ADF4113_SET_LD_PREC       1
#define ADF4113_SET_TEST          2
#define ADF4113_SET_ABP           3
#define ADF4113_SET_CP_GAIN       4
#define ADF4113_SET_POWER_DOWN    5
#define ADF4113_SET_CURRENT       6
#define ADF4113_SET_COUNTER_CONT  7
#define ADF4113_SET_FAST_LOCK     8
#define ADF4113_SET_CP_STATE      9
#define ADF4113_SET_PFD_SIGN      10
#define ADF4113_SET_MUX           11
#define ADF4113_SET_COUNTER_RESET 12
#define ADF4113_SET_CTL_BITS      13
#define ADF4113_SET_PRESCALE_VAL  14

// Function Set Value (mode)
// halSynSetFunc(uint8 funcSet, uint8 mode);
/* R Lacth (MSB->LSB) */
#define ADF4113_RESYNC_NORMAL1     0
#define ADF4113_RESYNC_NONDELAYRF  1
#define ADF4113_RESYNC_NORMAL2     2
#define ADF4113_RESYNC_DELAYRF     3
#define ADF4113_LD_PREC_3CYCLE     0
#define ADF4113_LD_PREC_5CYCLE     1
#define ADF4113_TEST_MODE_OFF      0
#define ADF4113_ANTI_WIDTH_3P0NS   0
#define ADF4113_ANTI_WIDTH_1P5NS   1
#define ADF4113_ANTI_WIDTH_6P0NS   2

/* N Lacth */
#define ADF4113_CP_GAIN1           0
#define ADF4113_CP_GAIN2           1

#define ADF4113_PDOWN_NORMAL    0
#define ADF4113_PDOWN_ASYNC     1
#define ADF4113_PDOWN_SYNC      3

#define ADF4113_ICP0            0
#define ADF4113_ICP1            1
#define ADF4113_ICP2            2
#define ADF4113_ICP3            3
#define ADF4113_ICP4            4
#define ADF4113_ICP5            5
#define ADF4113_ICP6            6
#define ADF4113_ICP7            7

#define ADF4113_TCOUNTER0       0
#define ADF4113_TCOUNTER1       1
#define ADF4113_TCOUNTER2       2
#define ADF4113_TCOUNTER3       3
#define ADF4113_TCOUNTER4       4
#define ADF4113_TCOUNTER5       5
#define ADF4113_TCOUNTER6       6
#define ADF4113_TCOUNTER7       7
#define ADF4113_TCOUNTER8       8
#define ADF4113_TCOUNTER9       9
#define ADF4113_TCOUNTER10      10
#define ADF4113_TCOUNTER11      11
#define ADF4113_TCOUNTER12      12
#define ADF4113_TCOUNTER13      13
#define ADF4113_TCOUNTER14      14
#define ADF4113_TCOUNTER15      15

#define ADF4113_FAST_LOCK_OFF   0
#define ADF4113_FAST_LOCK_MODE1 2
#define ADF4113_FAST_LOCK_MODE2 3

#define ADF4113_CP_OUT_NORMAL   0
#define ADF4113_CP_OUT_TRISTATE 1

#define ADF4113_PFD_NEGATIVE    0
#define ADF4113_PFD_POSITIVE    1

#define ADF4113_MUX_TRISTATE    0
#define ADF4113_MUX_DLD         1
#define ADF4113_MUX_DIVN_OUT    2
#define ADF4113_MUX_DVDD        3
#define ADF4113_MUX_DIVR_OUT    4
#define ADF4113_MUX_ALD         5
#define ADF4113_MUX_SERIAL_DATA 6
#define ADF4113_MUX_DGND        7

#define ADF4113_COUNTER_RESET_NORM   0
#define ADF4113_COUNTER_RESET_HOLD   1

// Error Code
#define HAL_SYN_OK                0x00
#define HAL_SYN_NOT_OK            0x01
#define HAL_SYN_INVALID_FUNC      0x02
#define HAL_SYN_NOT_CONFIGURED    0x03
#define HAL_SYN_INVALID_PRESCALE  0x04
#define HAL_SYN_INVALID_DIV_N     0x05

/***************************************************************************************************
 *                                             TYPEDEFS
 ***************************************************************************************************/
typedef struct
{ 
  uint8  control : 2;
  uint16 r_val : 14;
  uint8  abp : 2;
  uint8  testmode : 2;
  uint8  ldprec : 1;
  uint8  sync : 1;
  uint8  dly : 1;
  uint8  resrv : 1;
} halRCounter_t;

typedef struct
{ 
  uint8   control : 2;
  uint8   a_val : 6;
  uint16  b_val : 13;
  uint8   cpgain : 1;
  uint8   resrv : 2;
} halNCounter_t;

typedef struct
{ 
  uint8  control : 2;
  uint8  counter_rst : 1;
  uint8  pdown1 : 1;
  uint8  mux_control : 3;
  uint8  pfd_sign : 1;
  uint8  cp_out_state : 1;
  uint8  flock_en : 1;
  uint8  flock_mode : 1;
  uint8  counter_control : 4;
  uint8  current_set : 6;
  uint8  pdown2 : 1;
  uint8  prescale_val : 2;
} halFuncLatch_t;

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/
extern uint8 HalSynInit(void);
extern uint8 HalSynStart(uint16 rf_Freq);
extern uint8 halSynSetFunc(uint8 funcSet, uint8 mode);
extern void delayms(uint8 time);

/***************************************************************************************************
***************************************************************************************************/
#endif
