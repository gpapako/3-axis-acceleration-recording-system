/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    File name   : drv_hd44780_l.c
 *    Description : I/O function
 *
 *    History :
 *    1. Date        : February 5, 2009
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 31601 $
**************************************************************************/
#include "drv_hd44780_l.h"

/*************************************************************************
 * Function Name: HD44780_IO_Init
 * Parameters: none
 * Return: none
 * Description: Init IO ports directions and level
 *
 *************************************************************************/
void HD44780_IO_Init (void)
{
Int32U reg;
  // GPIO Init
  // Charge pump init, RST Line init
  reg  = GPIOA->CRH & ~( GPIO_CRH_MODE8  | GPIO_CRH_CNF8
                       | GPIO_CRH_MODE9  | GPIO_CRH_CNF9
                       | GPIO_CRH_MODE11 | GPIO_CRH_CNF11
                       );
  reg |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1 | GPIO_CRH_MODE11_1;
  GPIOA->CRH = reg;
  // E, RS, CS, Vdd, Data line init
  reg = GPIOB->CRH & ~( GPIO_CRH_MODE10 | GPIO_CRH_CNF10
                      | GPIO_CRH_MODE11 | GPIO_CRH_CNF11
                      | GPIO_CRH_MODE12 | GPIO_CRH_CNF12
                      | GPIO_CRH_MODE13 | GPIO_CRH_CNF13
                      | GPIO_CRH_MODE14 | GPIO_CRH_CNF14
                      | GPIO_CRH_MODE15 | GPIO_CRH_CNF15
                      );
  reg |= GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_1 | GPIO_CRH_MODE12_1
       | GPIO_CRH_MODE13_1 | GPIO_CRH_MODE14_1 | GPIO_CRH_MODE15_1
  ;
  GPIOB->CRH = reg;

  reg  = GPIOB->CRL & ~( GPIO_CRL_MODE0 | GPIO_CRL_CNF0
                       | GPIO_CRL_MODE1 | GPIO_CRL_CNF1
                       );
  reg |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1;
  GPIOB->CRL = reg;
#if HD4780_WR > 0
  // RW line init
  reg  = GPIOB->CRL & ~( GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
  reg |= GPIO_CRL_MODE2_1;
  GPIOB->CRL = reg;
#endif // HD4780_WR > 0

  HD44780SetPD();
}

/*************************************************************************
 * Function Name: HD44780SetPD
 * Parameters: none
 * Return: none
 * Description: Set power down
 *
 *************************************************************************/
void HD44780SetPD (void)
{
Int32U reg;
  // Disable charge pump
  reg  = GPIOA->CRH & ~( GPIO_CRH_MODE8  | GPIO_CRH_CNF8);
  reg |= GPIO_CRH_MODE8_1;
  GPIOA->CRH = reg;

  LCD_CHARGE_PUMP_EN_LOW();
  LCD_CHARGE_PUMP_LOW();

  // Stop Timer1 clock
  TIM1->CR1 &= ~TIM_CR1_CEN;
  RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;

  // CS Output - Low
  LCD_CS_LOW();
  // RS Output - Low
  LCD_RS_LOW();
  // E  Output - Low
  LCD_E_LOW();
  // Data lines Low
  LCD_DATA_PORT->BRR  = LCD_DATA << LCD_DATA_SHIFT;
#if HD4780_WR > 0
  // WR Output - Low
  LCD_RW_LOW();
#endif // HD4780_WR > 0
  // RST Output - Low
  LCD_RST_LOW();
  // Vdd Output - Low
  LCD_VDD_LOW();
}

/*************************************************************************
 * Function Name: HD44780SetPU
 * Parameters: none
 * Return: none
 * Description: Set power up
 *
 *************************************************************************/
void HD44780SetPU (void)
{
Int32U reg;

  // Vdd Output - High
  LCD_VDD_HIGH();
  HD44780_BUS_DLY();

  // CS Output - Low
  LCD_CS_LOW();
  // RS Output - Low
  LCD_RS_LOW();
  // E  Output - Low
  LCD_E_LOW();
#if HD4780_WR > 0
  // WR Output - Low
  LCD_RW_HIGH();
#endif // HD4780_WR > 0
  // RST Output - High
  LCD_RST_HIGH();

  // Enable charge pump
  // Enable Timer1 clock
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  reg  = GPIOA->CRH & ~( GPIO_CRH_MODE8  | GPIO_CRH_CNF8);
  reg |= GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1;
  GPIOA->CRH = reg;

  /* Time base configuration */
  TIM1->CR1 = 0;
  TIM1->PSC = 50;
  TIM1->ARR = LCD_CHRGE_PUMP_PER-1;
  // PWM1 Mode configuration: Channel1
  TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCR1  = LCD_CHRGE_PUMP_PER/2-1;
  TIM1->CCER  = TIM_CCER_CC1E;

  // Main Output Enable
  TIM1->BDTR &= ~TIM_BDTR_LOCK;
  TIM1->BDTR  =  TIM_BDTR_MOE;
  TIM1->CR1  |=  TIM_CR1_CEN;

  LCD_CHARGE_PUMP_EN_HIGH();
}

/*************************************************************************
 * Function Name: HD44780WrIO
 * Parameters: Int32U Data
 * Return: none
 * Description: Write to HD44780 I/O
 *
 *************************************************************************/
void HD44780WrIO (Int32U Data)
{

  // Write Data
#if HD4780_WR > 0
Int32U reg;
  LCD_RW_LOW();
  reg = GPIOB->CRH & ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12
                     | GPIO_CRH_MODE13 | GPIO_CRH_CNF13
                     | GPIO_CRH_MODE14 | GPIO_CRH_CNF14
                     | GPIO_CRH_MODE15 | GPIO_CRH_CNF15
                    );
  reg |= GPIO_CRH_MODE12_1
       | GPIO_CRH_MODE13_1
       | GPIO_CRH_MODE14_1
       | GPIO_CRH_MODE15_1
       ;
  GPIOB->CRH = reg;
#endif

  LCD_E_HIGH();
  LCD_DATA_PORT->BSRR = (  Data  & 0xF) << LCD_DATA_SHIFT;
  LCD_DATA_PORT->BRR  = ((~Data) & 0xF) << LCD_DATA_SHIFT;
  HD44780_BUS_DLY();
  LCD_E_LOW();
}

#if HD4780_WR > 0
/*************************************************************************
 * Function Name: HD44780RdIO
 * Parameters: none
 * Return: Int8U
 * Description: Read from HD44780 I/O
 *
 *************************************************************************/
Int8U HD44780RdIO (void)
{
Int8U Data;
Int32U reg;
  // Set Direction
  reg = GPIOB->CRH & ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12
                     | GPIO_CRH_MODE13 | GPIO_CRH_CNF13
                     | GPIO_CRH_MODE14 | GPIO_CRH_CNF14
                     | GPIO_CRH_MODE15 | GPIO_CRH_CNF15
                    );
  reg |= GPIO_CRH_CNF12_0
       | GPIO_CRH_CNF13_0
       | GPIO_CRH_CNF14_0
       | GPIO_CRH_CNF15_0
       ;
  GPIOB->CRH = reg;
  LCD_RW_HIGH();
  // Read Data
  LCD_E_HIGH();
  HD44780_BUS_DLY();
  reg = GPIOB->IDR;
  reg >>= LCD_DATA_SHIFT;
  Data  = reg & 0xF;
  LCD_E_LOW();
  return Data;
}
#endif
