/**************************************************************************************************
  Filename:       hal_types.h
  Revised:        $Date: 2013-10-18 15:20 $
  Revision:       $Revision: $
  Description:    Some useful typedef and definitions
**************************************************************************************************/

#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

typedef unsigned char   bool;


/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef HIGH
#define HIGH  1
#endif

#ifndef LOW
#define LOW   0
#endif

/**************************************************************************************************
 */
#endif
