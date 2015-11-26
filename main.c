
#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x_lib.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_it.h"
#include "drv_lisxxx.h"
#include "platform_config.h"
#include "msd.h"
#include "sddriver.h"
#include "const.h"

void RCC_Management(void);
void NVIC_Management(void);
void MMA_Management(void);
void TIM1_Management(void);
void Coordinates(void);
void Check_SD(void);
void MSD_Write_Block(u8* Buffer, u32 Address, u16 NumByteToWrite);

#define Data_Size 512
#define RCC_PLLSource_HSE_Div1           ((uint32_t)0x00010000)
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define OUTX_L 0x28
#define OUTX_H 0x29
#define OUTY_L 0x2A
#define OUTY_H 0x2B
#define OUTZ_L 0x2C
#define OUTZ_H 0x2D

unsigned char Data [Data_Size];
ErrorStatus HSE_Status;
char bufferx[20],buffery[20],bufferz[20];
int i=0,a=0,l;

  
int main(void){
for(l=0;l<21;l++)
bufferx[l]=buffery[l]=bufferz[l]=0;

RCC_Management();
NVIC_Management();

SysTick_SetReload(9000);//calibration gia to eswteriko roloi tou Cortex M-3(max HCLK/8)
SysTick_ITConfig(ENABLE);//energopoihsh twn interrupts

MMA_Management();

LCDInit();//arxikopoihsh ths LCD(emperiexetai h arxikopoihsh ths SPI thyras)
LCDContrast(0x45);

TIM1_Management();

}

void RCC_Management(void)//synarthsh pou asxoleitai me thn energopoihsh olwn twn aparaithtwn rologiwn kai kanei reset opou xreiazetai
{
RCC_DeInit(); //synarthsh pou kanei deinitialization stous kataxwrhtes RCC twn perifereiakwn
RCC_HSEConfig(RCC_HSE_ON); //energopoiei to HSE
HSE_Status = RCC_WaitForHSEStartUp(); // metavlhth pou pairnei tis times SUCCESS h' ERROR
if(HSE_Status == SUCCESS)
{
RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//to PLL tha einai 8*9=72 MHz
RCC_PLLCmd(ENABLE);//energopoihsh tou PLL
while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)//perimenoume thn energopoihsh tou PLL
{
}
RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//thetoume to PLL ws system clock
while(RCC_GetSYSCLKSource() != 0x08)//perimenoume mexri to PLL na ginei system clock
{
}
FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); 
FLASH_SetLatency(FLASH_Latency_2); // 2 wait states otan to system clock einai anamesa se 48 kai 72 MHz
RCC_HCLKConfig(RCC_SYSCLK_Div1); // to roloi tou AHB tha einai 72 MHz
RCC_PCLK2Config(RCC_HCLK_Div1); // to roloi tou APB2 tha einai 72 MHz(max 72 MHz)
RCC_PCLK1Config(RCC_HCLK_Div2); // to roloi tou APB1 tha einai 72/2=36MHz(max 36 MHz)
}

}

void NVIC_Management(void)//synarthsh pou asxoleitai me thn e3uphrethsh kai thn proteraiothta twn routinwn diakophs
{
NVIC_InitTypeDef NVIC_Struct;//struct pou periexei shmantika stoixeia gia tis diakopes mas
NVIC_Struct.NVIC_IRQChannel = 25 ; // kanali pou e3yphretei diakopes apo ton TIM1
NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;//oso pio mikrh timh toso pio amesa tha e3yphretithei h diakoph
NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;//energopoihsh
NVIC_Init(&NVIC_Struct);//synarthsh pou arxikopoiei ta NVIC peripherals symfwna me tis parametrous pou thesame pio panw

#ifdef VECT_TAB_RAM
NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x20000000); // synarthsh pou thetei to base location tou vector table(0x200 gia thn SRAM)
#else
NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08000000); 
#endif
  
}


void MMA_Management(void)//synarthsh pou asxoleitai me th thyra Genikou Skopou B me thn opoia sundeetai to accelerometro
{
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//energopoihsh tou rologiou gia thn GPIOB

GPIO_InitTypeDef GPIO_Struct;//struct pou periexei plhrofories gia thn GPIOB
GPIO_Struct.GPIO_Pin = GPIO_Pin_5;
GPIO_Struct.GPIO_Mode = GPIO_Mode_IPD;
GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_Struct);//synarthsh pou arxikopoiei thn GPIOB symfwna me tis parametrous pou thesame pio panw

Hrd_I2C_Init();//synarthsh pou kanei arxikopoihsh sto i2c interface

WriteReg(0xC7, CTRL_REG1); //energopoihsh tou sygkekrimenou kataxwrhth
WriteReg(0x54, CTRL_REG2); // 
WriteReg(0x00, CTRL_REG3); // 

}


void TIM1_Management(void)//synarthsh pou asxoleitai me ton timer 1 o opoios tha prokalei kai th diakoph
{
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//energopoihsh tou rologiou tou TIM1
 
 TIM_TimeBaseInitTypeDef TIM1_Struct;//Struct pou periexei plhrofories gia ton Timer 1
 TIM1_Struct.TIM_Period = 10000;//h overflow value tou timer
 TIM1_Struct.TIM_Prescaler = 72; //me prescaler 72 tha exoume interrupt kathe 0,01 sec
 TIM1_Struct.TIM_ClockDivision = 0;
 TIM1_Struct.TIM_CounterMode = TIM_CounterMode_Up;//upcounting
 TIM_TimeBaseInit(TIM1, &TIM1_Struct);//arxikopoiei ton timer 1 symfwna me tis parametrous pou thesame pio panw
 
 TIM_ITConfig(TIM1,TIM_IT_Update, ENABLE);//energopoihsh diakopwn apo ton timer
 TIM_Cmd(TIM1,ENABLE);//energopoihsh tou timer

}

void TIM1_UP_IRQHandler(void)//routina diakophs gia diakoph pou prokaleitai apo upcounting tou timer 1
{
TIM_Cmd(TIM1,DISABLE);//apenergopoihsh tou timer
Coordinates();//synarthsh pou pairnei ta dedomena apo to accelerometro kai ta apothikeuei se pinakes


TIM_ClearFlag(TIM1,TIM_FLAG_Update); //kanei clear to flag ths diakophs
TIM_Cmd(TIM1,ENABLE);//energopoihsh tou timer
TIM_SetCounter (TIM1,0x00); //3anarxizei to upcounting apo to 0

}

void Coordinates(void){
  unsigned char numberl,numberh;
  int number;

  
  number = numberh = numberl = 0;
ReadReg(&numberl, OUTX_L);//diavazei ta 8 ligotero shmantika pshfia ths x syntetagmenhs
ReadReg(&numberh, OUTX_H);//diavazei ta 8 pio shmantika pshfia ths x syntetagmenhs
number = (numberh<<8)|numberl;//olisthish 8 pshfiwn pros ta aristera gia ton arithmo me ta 8 pio shmantika pshfia kai me ena or pairnoume ton 16diko pou theloume
sprintf(&bufferx[i], "X acceleration:0x%04x\0", number);//apothikeuetai sto stoixeio i tou pinaka h x syntetagmenh
Data[a]=number;
a++;

 number = numberh = numberl = 0;
ReadReg(&numberl, OUTY_L);
ReadReg(&numberh, OUTY_H);
number = (numberh<<8)|numberl;
sprintf(&buffery[i], "Y acceleration:0x%04x\0", number);
Data[a]=number;
a++;

 number = numberh = numberl = 0;
ReadReg(&numberl, OUTZ_L);
ReadReg(&numberh, OUTZ_H);
number = (numberh<<8)|numberl;
sprintf(&bufferz[i], "Z acceleration:0x%04x\0", number);
Data[a]=number;
a++;

LCDClear();//katharizei thn LCD
LCDStr(0,(unsigned char*)bufferx[i],0);//stelnei kapoio string sthn LCD(edw sth thesh (0,0)
LCDStr(1,(unsigned char*)buffery[i],0);//stelnei kapoio string sthn LCD(edw sth thesh (1,0)
LCDStr(2,(unsigned char*)bufferz[i],0);//stelnei kapoio string sthn LCD(edw sth thesh (2,0)
LCDUpdate();//kanei update sth mnhmh ths lcd
i++; 

if (a==Data_Size-1){
Check_SD();//synarthsh pou kanei elegxo gia thn SD
a=0;
}
 
}


void Check_SD(void){
unsigned char x,y;
int delay;

x=CardIsPresent();
while(x==1){
LCDClear();
LCDStr(0,"Put SD Card",0);
for(delay=0;delay<50000;delay++){
}
x=CardIsPresent();
}

y=CardIsProtected();
if(y==1){
LCDClear();
LCDStr(0,"Card is",0);
LCDStr(1,"protected",0);
}
else MSD_Write_Block(Data,0x00,512);
}

void MSD_Write_Block(u8* Buffer, u32 Address, u16 NumByteToWrite)
{
RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//energopoihsh tou rologiou gia to SPI2
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//energopoihsh tou rologiou gia to GPIOD

  SPI_InitTypeDef  SPI_Struct;//Struct gia to SPI2
  GPIO_InitTypeDef GPIO_Struct;//Struct gia to GPIOD
  
  GPIO_Struct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;//alternate function push-pull
  GPIO_Init(GPIOD, &GPIO_Struct);

  GPIO_Struct.GPIO_Pin = GPIO_Pin_0;
  GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;//output push-pull
  GPIO_Init(GPIOD, &GPIO_Struct);
  
  GPIO_WriteBit(GPIOD, GPIO_Pin_0,Bit_SET);//kanei set to bit tou pin 0

  SPI_Struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//to spi tha leitourgei kai pros tis 2 kateuthynseis
  SPI_Struct.SPI_Mode = SPI_Mode_Master;//tha leitourgei se mode afenth(master)
  SPI_Struct.SPI_DataSize = SPI_DataSize_8b;
  SPI_Struct.SPI_CPOL = SPI_CPOL_High;
  SPI_Struct.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_Struct.SPI_NSS = SPI_NSS_Soft;
  SPI_Struct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_Struct.SPI_FirstBit = SPI_FirstBit_MSB;//to prwto bit tha einai to pio shmantiko bit(most significant
  SPI_Struct.SPI_CRCPolynomial = 7;//kathorizei to polywnymo gia to CRC
  SPI_Init(SPI2, &SPI_Struct);  //arxikopoiei to struct symfwna me tis parametrous pou thesame pio panw
  SPI_Cmd(SPI2,ENABLE);//energopoihsh tou SPI2 
  GPIO_ResetBits(GPIOD, GPIO_Pin_0);//reset tou bit tou pin 0
  SD_WriteBlock(Address, NumByteToWrite, Buffer);
}
