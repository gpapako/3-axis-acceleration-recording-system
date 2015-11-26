/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    File name   : drv_hd44780_l.h
 *    Description : STM32 HD44780 low level I/O function driver
 *
 *    History :
 *    1. Date        : February 5, 2009
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 31601 $
 **************************************************************************/
#include "includes.h"

#ifndef  __DRV_HD44780_L_H
#define  __DRV_HD44780_L_H

#define LCD_RST                   (1UL << 11)
#define LCD_RST_PORT              GPIOA

#define LCD_CHARGE_PUMP_EN        (1UL << 9)
#define LCD_CHARGE_PUMP_EN_PORT   GPIOA

#define LCD_CHARGE_PUMP           (1UL << 8)
#define LCD_CHARGE_PUMP_PORT      GPIOA

#define LCD_RS                    (1UL << 1)
#define LCD_RS_PORT               GPIOB

#define LCD_E                     (1UL << 0)
#define LCD_E_PORT                GPIOB

#define LCD_RW                    (1UL << 2)
#define LCD_RW_PORT               GPIOB

#define LCD_CS                    (1UL << 11)
#define LCD_CS_PORT               GPIOB

#define LCD_VDD                   (1UL << 10)
#define LCD_VDD_PORT              GPIOB

#define LCD_DATA                  0xFUL
#define LCD_DATA_SHIFT            12
#define LCD_DATA_PORT             GPIOB

#define LCD_RST_HIGH()            LCD_RST_PORT->BSRR = LCD_RST
#define LCD_RST_LOW()             LCD_RST_PORT->BRR = LCD_RST

#define LCD_CS_HIGH()             LCD_CS_PORT->BSRR = LCD_CS
#define LCD_CS_LOW()              LCD_CS_PORT->BRR = LCD_CS

#define LCD_E_HIGH()              LCD_E_PORT->BSRR = LCD_E
#define LCD_E_LOW()               LCD_E_PORT->BRR = LCD_E

#define LCD_VDD_HIGH()            LCD_VDD_PORT->BSRR = LCD_VDD
#define LCD_VDD_LOW()             LCD_VDD_PORT->BRR = LCD_VDD

#define LCD_RS_HIGH()             LCD_RS_PORT->BSRR = LCD_RS
#define LCD_RS_LOW()              LCD_RS_PORT->BRR = LCD_RS

#define LCD_RW_HIGH()             LCD_RW_PORT->BSRR = LCD_RW
#define LCD_RW_LOW()              LCD_RW_PORT->BRR = LCD_RW

#define LCD_CHARGE_PUMP_EN_HIGH() LCD_CHARGE_PUMP_EN_PORT->BSRR = LCD_CHARGE_PUMP_EN
#define LCD_CHARGE_PUMP_EN_LOW()  LCD_CHARGE_PUMP_EN_PORT->BRR = LCD_CHARGE_PUMP_EN

#define LCD_CHARGE_PUMP_HIGH()    LCD_CHARGE_PUMP_PORT->BSRR = LCD_CHARGE_PUMP
#define LCD_CHARGE_PUMP_LOW()     LCD_CHARGE_PUMP_PORT->BRR = LCD_CHARGE_PUMP

// LCD voltage doubler period
#define LCD_CHRGE_PUMP_PER        10

/* Delay macro */
#define HD44780_BUS_DLY()         for(volatile int dly = 10;dly;--dly)

void HD44780_IO_Init (void);
void HD44780SetPD (void);
void HD44780SetPU (void);
void HD44780WrIO (Int32U);
#if HD4780_WR > 0
Int8U HD44780RdIO (void);
#endif // HD4780_WR > 0

#endif  /* __DRV_HD44780_L_H */
