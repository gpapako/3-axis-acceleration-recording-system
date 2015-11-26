/**********************************************************************************/
/*                                                                                */
/*    Copyright (C) 2005 OLIMEX  LTD.                                             */
/*                                                                                */
/*    Module Name    :  i2c module                                                */
/*    File   Name    :  i2c.c                                                     */
/*    Revision       :  01.00                                                     */
/*    Date           :  2005/07/04 initial version                                */
/*                                                                                */
/**********************************************************************************/
#include "i2c.h"

void _NOP(){
 volatile int i = 5;
 for(;i;--i);
}



 GPIO_InitTypeDef GPIO_InitStructure;

// SCL -------------------------------------------------------------------------
void SCL_DIR(char state)  {
 if(state) {
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  else {
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
}

void SCL_OUT(char state) {
  if(state) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
  }
  else {
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
  }
}



// SDA -------------------------------------------------------------------------
void SDA_DIR(char state)  {
  if(state) {
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  else {
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
}

void SDA_OUT(char state) {
  if(state) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
  }
  else {
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
  }
}

char SDA_IN(void) {
   if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)) == Bit_SET)
     return 1;
   else
     return 0;
}

#define SCL_HIGH  SCL_OUT(1);
#define SCL_LOW   SCL_OUT(0);
#define SDA_HIGH  SDA_DIR(0);
#define SDA_LOW   SDA_DIR(1);



/****************************************************************************/
/*  Initialize I2C interface                                                */
/*  Function : Hrd_I2C_Init                                                 */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
void Hrd_I2C_Init(){
  SCL_OUT(0);
  SCL_DIR(1);
  SDA_DIR(0);
  SDA_OUT(0);
}


/****************************************************************************/
/*  Start Conditional for I2C                                               */
/*  Function : Hrd_I2C_StartCond                                            */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
void Hrd_I2C_StartCond(){

  SCL_LOW;
  _NOP();
  SDA_HIGH;
  _NOP();
  SCL_HIGH;
  _NOP();
  SDA_LOW;
  _NOP();
  SCL_LOW;
}


/****************************************************************************/
/*  Stop Conditional for I2C                                                */
/*  Function : Hrd_I2C_StopCond                                             */
/*      Parameters                                                          */
/*          Input   :   Nothing                                             */
/*          Output  :   Nothing                                             */
/****************************************************************************/
void Hrd_I2C_StopCond(){

  SCL_LOW;
  SDA_LOW;
  _NOP();
  SCL_HIGH;
  _NOP();
  SDA_HIGH;
}


/****************************************************************************/
/*  Write Byte to I2C                                                       */
/*  Function : Hrd_I2C_WriteByte                                            */
/*      Parameters                                                          */
/*          Input   :   character to write                                  */
/*          Output  :   acknowledge                                         */
/****************************************************************************/
I2C_AKN_DEF Hrd_I2C_WriteByte  (unsigned char ch){

  unsigned int i = 0;
  for(i = 8; i; --i)
  {
    SCL_LOW;
    if(ch&0x80) {
      SDA_HIGH;
    }
    else {
      SDA_LOW;
    }
    _NOP();
    SCL_HIGH;
    ch <<= 1;
    _NOP();
    SCL_LOW;
  }
  SDA_HIGH;
  _NOP();
  SCL_HIGH;
  _NOP();
  i = SDA_IN() ? I2C_NAK: I2C_AKN;
  SCL_LOW;
  return((I2C_AKN_DEF)i);
}


/****************************************************************************/
/*  Read Byte from I2C                                                      */
/*  Function : Hrd_I2C_ReadByte                                             */
/*      Parameters                                                          */
/*          Input   :   need acknowledge                                    */
/*          Output  :   read character                                      */
/****************************************************************************/
unsigned char Hrd_I2C_ReadByte (I2C_AKN_DEF  Akn){

  unsigned char ch =0;
  unsigned int i = 0;
  SDA_HIGH;

  for(i = 8; i; --i)
  {
    SCL_HIGH;
    ch <<=1;
    ch |= SDA_IN() ? 1:0;
    SCL_LOW;
  }
  if (Akn) SDA_LOW;

  SCL_HIGH;
  _NOP();
  SCL_LOW;

  return ch;
}

