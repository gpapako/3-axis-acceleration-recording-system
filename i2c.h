/**********************************************************************************/
/*                                                                                */
/*    Copyright (C) 2005 OLIMEX  LTD.                                             */
/*                                                                                */
/*    Module Name    :  i2c module                                                */
/*    File   Name    :  i2c.h                                                     */
/*    Revision       :  01.00                                                     */
/*    Date           :  2005/07/04 initial version                                */
/*                                                                                */
/**********************************************************************************/

#ifndef I2C_def
#define I2C_def

#include "stm32f10x_lib.h"

/******************************************/
/*            definitions                 */
/******************************************/
// #define SCL_DIR   P2DIR_bit.P2DIR_0
// #define SCL_OUT   P2OUT_bit.P2OUT_0
//
// #define SDA_DIR   P2DIR_bit.P2DIR_1
// #define SDA_OUT   P2OUT_bit.P2OUT_1
// #define SDA_IN    P2IN_bit.P2IN_1
//
// #define SCL_HIGH  SCL_OUT = 1
// #define SCL_LOW   SCL_OUT = 0
// #define SDA_HIGH  SDA_DIR = 0
// #define SDA_LOW   SDA_DIR = 1



typedef enum
{
  I2C_NAK = 0,
  I2C_AKN = 1,
}I2C_AKN_DEF;


/****************************************************************************/
/*  Initialize I2C interface                                                */
/*  Function : Hrd_I2C_Init                                                 */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
void Hrd_I2C_Init();


/****************************************************************************/
/*  Start Conditional for I2C                                               */
/*  Function : Hrd_I2C_StartCond                                            */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
void Hrd_I2C_StartCond();


/****************************************************************************/
/*  Stop Conditional for I2C                                                */
/*  Function : Hrd_I2C_StopCond                                             */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
void Hrd_I2C_StopCond();


/****************************************************************************/
/*  Write Byte to I2C                                                       */
/*  Function : Hrd_I2C_WriteByte                                            */
/*      Parameters                                                          */
/*          Input   :   character to write                                  */
/*          Output  :   acknowledge                                         */
/****************************************************************************/
I2C_AKN_DEF Hrd_I2C_WriteByte  (unsigned char ch);


/****************************************************************************/
/*  Read Byte from I2C                                                      */
/*  Function : Hrd_I2C_ReadByte                                             */
/*      Parameters                                                          */
/*          Input   :   need acknowledge                                    */
/*          Output  :   read character                                      */
/****************************************************************************/
unsigned char Hrd_I2C_ReadByte (I2C_AKN_DEF  Akn);

#endif
