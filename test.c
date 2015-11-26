#include "test.h"
#include "mmc.h"
#include "stm32f10x_lib.h"
#include <string.h>
//#include "bits.h"

// mmc variable
extern char mmc_buffer[512];
char mmc_buffer_test_1[512];
char state_mmc = 1;

// ext variable
char state_ext = 1;
char i=0;

extern GPIO_InitTypeDef GPIO_InitStructure;

void DelayN (unsigned long a) { while (--a!=0); }

unsigned char CardIsPresent(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  // CP
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)) != Bit_SET)
    return 0; // Card present
	else return 1;
}

unsigned char CardIsProtected(void)
{
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  // WP
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)) != Bit_SET)
    return 1; //car write protected
	else return 0;
}

// MMC ======================================================================================
unsigned char TestMMC(void) {

  // return value - 0 - mmc error
  // return value - 1 - mmc test ok
  // return value - 2 - mmc card not present
  // return value - 3 - mmc card write protect

  // set success
  state_mmc = 1;

  // Enable clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  // WP
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // CP
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)) != Bit_SET)
    return 3; // Card not present

  if((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)) != Bit_SET)
    return 2; //car write protected

  if (initMMC() == MMC_SUCCESS)	// card found
  {
    //card_state |= 1;
    memset(&mmc_buffer,0,512);
    mmcReadRegister (10, 16);
    mmc_buffer[7]=0;

    // Fill first Block (0) with 'A'
    memset(&mmc_buffer,'0',512);    //set breakpoint and trace mmc_buffer contents
    mmcWriteBlock(0);
    // Fill second Block (1)-AbsAddr 512 with 'B'
    memset(&mmc_buffer,'1',512);
    mmcWriteBlock(512);

    // Read first Block back to buffer
    memset(&mmc_buffer,0x00,512);
    mmcReadBlock(0,512);
    memset(&mmc_buffer_test_1,'0',512);
    if(strncmp(&mmc_buffer[0], &mmc_buffer_test_1[0], 512)) state_mmc=0;

    // Read first Block back to buffer
    memset(&mmc_buffer,0x00,512);
    mmcReadBlock(512,512);
    memset(&mmc_buffer_test_1,'1',512);
    if(strncmp(&mmc_buffer[0], &mmc_buffer_test_1[0], 512)) state_mmc=0;

    memset(&mmc_buffer,0x00,512);
  }

  return state_mmc;
}


// EXT =========================================================================================
/*const unsigned int mask_port_a  = 0x060C&(~BIT2);  // pullup for EXT1 -> PA2
const unsigned int mask_port_b  = 0x0F03&(~BIT0);  // pullup for EXT2 -> PB0
const unsigned int mask_port_a_uext  = BIT2|BIT3;
const unsigned int mask_port_b_uext  = BIT10|BIT11|BIT12|BIT14|BIT15;
const unsigned int mask_port_c  = 0x0007;
const unsigned int mask_port_d  = 0x0004;


void Stop(void) {

}

void AllAsInput(void) {

  // PortA
  GPIOA->CRL = 0x44444444;
  GPIOA->CRH = 0x44444444;

  // PortB
  GPIOB->CRL = 0x44444444;
  GPIOB->CRH = 0x44444444;

  // PortC
  GPIOC->CRL = 0x44444444;
  GPIOC->CRH = 0x44444444;

  // PortD
  GPIOD->CRL = 0x44444444;
  GPIOD->CRH = 0x44444444;

}

void PinAsOutputLowPortA(char pin) {

  switch (pin) {
    case  0: GPIOA->CRL = 0x44444343; GPIOA->BRR = 0x00000001; break;
    case  1: GPIOA->CRL = 0x44444334; GPIOA->BRR = 0x00000002; break;
    case  2: GPIOA->CRL = 0x44444344; GPIOA->BRR = 0x00000004; break;
    case  3: GPIOA->CRL = 0x44443344; GPIOA->BRR = 0x00000008; break;
    case  4: GPIOA->CRL = 0x44434344; GPIOA->BRR = 0x00000010; break;
    case  5: GPIOA->CRL = 0x44344344; GPIOA->BRR = 0x00000020; break;
    case  6: GPIOA->CRL = 0x43444344; GPIOA->BRR = 0x00000040; break;
    case  7: GPIOA->CRL = 0x34444344; GPIOA->BRR = 0x00000080; break;
    case  8: GPIOA->CRH = 0x44444443; GPIOA->BRR = 0x00000100; break;
    case  9: GPIOA->CRH = 0x44444434; GPIOA->BRR = 0x00000200; GPIOA->CRL = 0x44444344; break;
    case 10: GPIOA->CRH = 0x44444344; GPIOA->BRR = 0x00000400; break;
    case 11: GPIOA->CRH = 0x44443444; GPIOA->BRR = 0x00000800; break;
    case 12: GPIOA->CRH = 0x44434444; GPIOA->BRR = 0x00001000; break;
    case 13: GPIOA->CRH = 0x44344444; GPIOA->BRR = 0x00002000; break;
    case 14: GPIOA->CRH = 0x43444444; GPIOA->BRR = 0x00004000; break;
    case 15: GPIOA->CRH = 0x34444444; GPIOA->BRR = 0x00008000; break;
  }
}

void PinAsOutputLowPortB(char pin) {

  switch (pin) {
    case  0: GPIOB->CRL = 0x44444443; GPIOB->BRR = 0x00000001; break;
    case  1: GPIOB->CRL = 0x44444433; GPIOB->BRR = 0x00000002; break;
    case  2: GPIOB->CRL = 0x44444343; GPIOB->BRR = 0x00000004; break;
    case  3: GPIOB->CRL = 0x44443443; GPIOB->BRR = 0x00000008; break;
    case  4: GPIOB->CRL = 0x44434443; GPIOB->BRR = 0x00000010; break;
    case  5: GPIOB->CRL = 0x44344443; GPIOB->BRR = 0x00000020; break;
    case  6: GPIOB->CRL = 0x43444443; GPIOB->BRR = 0x00000040; break;
    case  7: GPIOB->CRL = 0x34444443; GPIOB->BRR = 0x00000080; break;
    case  8: GPIOB->CRH = 0x44444443; GPIOB->BRR = 0x00000100; GPIOB->CRL = 0x44444443; break;
    case  9: GPIOB->CRH = 0x44444434; GPIOB->BRR = 0x00000200; break;
    case 10: GPIOB->CRH = 0x44444344; GPIOB->BRR = 0x00000400; break;
    case 11: GPIOB->CRH = 0x44443444; GPIOB->BRR = 0x00000800; break;
    case 12: GPIOB->CRH = 0x44434444; GPIOB->BRR = 0x00001000; break;
    case 13: GPIOB->CRH = 0x44344444; GPIOB->BRR = 0x00002000; break;
    case 14: GPIOB->CRH = 0x43444444; GPIOB->BRR = 0x00004000; break;
    case 15: GPIOB->CRH = 0x34444444; GPIOB->BRR = 0x00008000; break;
  }
}


void PinAsOutputLowPortC(char pin) {

  switch (pin) {
    case  0: GPIOC->CRL = 0x44444443; GPIOC->BRR = 0x00000001; break;
    case  1: GPIOC->CRL = 0x44444434; GPIOC->BRR = 0x00000002; break;
    case  2: GPIOC->CRL = 0x44444344; GPIOC->BRR = 0x00000004; break;
    case  3: GPIOC->CRL = 0x44443444; GPIOC->BRR = 0x00000008; break;
    case  4: GPIOC->CRL = 0x44434444; GPIOC->BRR = 0x00000010; break;
    case  5: GPIOC->CRL = 0x44344444; GPIOC->BRR = 0x00000020; break;
    case  6: GPIOC->CRL = 0x43444444; GPIOC->BRR = 0x00000040; break;
    case  7: GPIOC->CRL = 0x34444444; GPIOC->BRR = 0x00000080; break;
    case  8: GPIOC->CRH = 0x44444443; GPIOC->BRR = 0x00000100; GPIOC->CRL = 0x44444444; break;
    case  9: GPIOC->CRH = 0x44444434; GPIOC->BRR = 0x00000200; break;
    case 10: GPIOC->CRH = 0x44444344; GPIOC->BRR = 0x00000400; break;
    case 11: GPIOC->CRH = 0x44443444; GPIOC->BRR = 0x00000800; break;
    case 12: GPIOC->CRH = 0x44434444; GPIOC->BRR = 0x00001000; break;
    case 13: GPIOC->CRH = 0x44344444; GPIOC->BRR = 0x00002000; break;
    case 14: GPIOC->CRH = 0x43444444; GPIOC->BRR = 0x00004000; break;
    case 15: GPIOC->CRH = 0x34444444; GPIOC->BRR = 0x00008000; break;
  }

}

void PinAsOutputLowPortD(char pin) {

  switch (pin) {
    case  0: GPIOD->CRL = 0x44444443; GPIOD->BRR = 0x00000001; break;
    case  1: GPIOD->CRL = 0x44444434; GPIOD->BRR = 0x00000002; break;
    case  2: GPIOD->CRL = 0x44444344; GPIOD->BRR = 0x00000004; break;
    case  3: GPIOD->CRL = 0x44443444; GPIOD->BRR = 0x00000008; break;
    case  4: GPIOD->CRL = 0x44434444; GPIOD->BRR = 0x00000010; break;
    case  5: GPIOD->CRL = 0x44344444; GPIOD->BRR = 0x00000020; break;
    case  6: GPIOD->CRL = 0x43444444; GPIOD->BRR = 0x00000040; break;
    case  7: GPIOD->CRL = 0x34444444; GPIOD->BRR = 0x00000080; break;
    case  8: GPIOD->CRH = 0x44444443; GPIOD->BRR = 0x00000100; GPIOD->CRL = 0x44444444; break;
    case  9: GPIOD->CRH = 0x44444434; GPIOD->BRR = 0x00000200; break;
    case 10: GPIOD->CRH = 0x44444344; GPIOD->BRR = 0x00000400; break;
    case 11: GPIOD->CRH = 0x44443444; GPIOD->BRR = 0x00000800; break;
    case 12: GPIOD->CRH = 0x44434444; GPIOD->BRR = 0x00001000; break;
    case 13: GPIOD->CRH = 0x44344444; GPIOD->BRR = 0x00002000; break;
    case 14: GPIOD->CRH = 0x43444444; GPIOD->BRR = 0x00004000; break;
    case 15: GPIOD->CRH = 0x34444444; GPIOD->BRR = 0x00008000; break;
  }

}

void PullUpHigh(void) {

  // PB0 => output, high
  GPIOB->CRL  = 0x44444443;
  GPIOB->BSRR = 0x00000001;

  // PA2 => output, high
  GPIOA->CRL  = 0x44444344;
  GPIOA->BSRR = 0x00000004;

}

void PullUpLow(void) {

  // PB0 => output, low
  GPIOB->CRL  = 0x44444443;
  GPIOB->BRR  = 0x00000001;

  // PA2 => output, low
  GPIOA->CRL  = 0x44444344;
  GPIOA->BRR  = 0x00000004;

}

unsigned char TestEXT(void) {

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

  return 1;	//Always return success
}


void PullUpHighUEXT(void) {

  // PB13 => output, high
  GPIOB->CRL  = 0x44444444;
	GPIOB->CRH  = 0x44344444;
  GPIOB->BSRR = 1<<13;

}

void PullUpLowUEXT(void) {

  // PB13 => output, low
  GPIOB->CRL  = 0x44444444;
	GPIOB->CRH  = 0x44344443;
  GPIOB->BRR  = 1<<13;
}

void PinAsOutputLowPortA_UEXT(char pin) {

  switch (pin) {
    case  2: GPIOA->CRL = 0x44444344; GPIOA->BRR = 0x00000004; break;
    case  3: GPIOA->CRL = 0x44443444; GPIOA->BRR = 0x00000008; break;
  }
}

void PinAsOutputLowPortB_UEXT(char pin) {

  switch (pin) {
    case 10: GPIOB->CRH = 0x44344344; GPIOB->BRR = 0x00000400; break;
    case 11: GPIOB->CRH = 0x44343444; GPIOB->BRR = 0x00000800; break;
    case 12: GPIOB->CRH = 0x44334444; GPIOB->BRR = 0x00001000; break;
    case 13: GPIOB->CRH = 0x44344444; GPIOB->BRR = 0x00002000; break;
    case 14: GPIOB->CRH = 0x43344444; GPIOB->BRR = 0x00004000; break;
    case 15: GPIOB->CRH = 0x34344444; GPIOB->BRR = 0x00008000; break;
  }
}

unsigned char TestUEXT(void) {

	unsigned int tempa, tempb;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

  return 1; //Always return success
}



*/