#include "fifo.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <stdio.h>


 /* Test data arrays */
unsigned char data_out[32];
unsigned char data_in[32];

char str[256];


/* GPIO_Configuration */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure LEDs connected to PC.8, PC.9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/* USART_Configuration */
void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

	/* Fill USART_InitStructure with default values
	* (9600, 8 bit, 1 stop bit, no flow control) */
	USART_StructInit(&USART_InitStructure);
	/* Set baudrate to 115200 */
	USART_InitStructure.USART_BaudRate = 115200;

	/* Init USART2 */
	USART_Init(USART2, &USART_InitStructure);
}


/* NVIC_Configuration */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/* RCC_Configuration */
void RCC_Configuration(void)
{
	/* Enable USART2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Enable GPIOA and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
}

/* Print string over UART */
void UART_PrintStr(char *str) {
	while (*str) {
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) {
		}
		USART_SendData(USART2, *str++);
	}
}


/* Print data over UART */
void UART_PrintData(unsigned char *data, unsigned int n)
{
	char str[16];

	UART_PrintStr("[ ");
	while (n--) {
		sprintf(str, "%d ", *data++);
		UART_PrintStr(str);
	}
	UART_PrintStr("]\r\n");
}


/* Test routine, print fifo buffer state */
void UART_PrintBufferState(void)
{
	sprintf(str, "Data in buffer: %d\r\n", GetBufDataCount());
	UART_PrintStr(str);
	UART_PrintStr("buffer = ");
	LookBuf(data_out);
	UART_PrintData(data_out, FIFO_BUF_SIZE);
	sprintf(str, "overflow_flag - %d, empty_flag - %d\r\n", \
				buf_flags & overflow_flag, (buf_flags & empty_flag) >> 1);
	UART_PrintStr(str);
	UART_PrintStr("\r\n");
}


int main(void)
{
	unsigned char i;

	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	USART_Configuration();

	/* Enable USART2 */
	USART_Cmd(USART2, ENABLE);

	for (i = 0; i < 32; i++) {
		data_out[i] = i;
	}

	/* Fifo buffer routines tests (FIFO_BUF_SIZE == 16) */

	/* Writing some data */
	UART_PrintStr("# Writing some data.\r\n");
	sprintf(str, "Wrote data: %d\r\n", WriteBuf(data_out, 12));
	UART_PrintStr(str);
	UART_PrintBufferState();

	/* Reading some data */
	UART_PrintStr("# Reading some data.\r\n");
	sprintf(str, "Read data: %d\r\n", ReadBuf(data_in, 2));
	UART_PrintStr(str);
	UART_PrintBufferState();

	/* Data in buffer == FIFO_BUF_SIZE, overflow_flag is set */
	UART_PrintStr("# Data in buffer == FIFO_BUF_SIZE, overflow_flag is set.\r\n");
	sprintf(str, "Wrote data: %d\r\n", WriteBuf(data_out, 6));
	UART_PrintStr(str);
	UART_PrintBufferState();

	/* Buffer is overflowed, new data dropped */
	UART_PrintStr("# Buffer is overflowed, new data dropped.\r\n");
	sprintf(str, "Wrote data: %d\r\n", WriteBuf(data_out, 1));
	UART_PrintStr(str);
	UART_PrintBufferState();

	/* Buffer is overflowed, read all data, empty_flag is set */
	UART_PrintStr("# Buffer is overflowed, read all data, empty_flag is set.\r\n");
	sprintf(str, "Read data: %d\r\n", ReadBuf(data_in, 16));
	UART_PrintStr(str);
	UART_PrintBufferState();

	/* Buffer is emty, write some data, empty_flag is cleared */
	UART_PrintStr("# Buffer is emty, write some data, empty_flag is cleared.\r\n");
	sprintf(str, "Wrote data: %d\r\n", WriteBuf(data_out, 1));
	UART_PrintStr(str);
	UART_PrintBufferState();

	/* Flush buffer, empty_flag is set */
	UART_PrintStr("# Flush buffer, empty_flag is set.\r\n");
	FlushBuf();
	UART_PrintBufferState();

    while(1)
    {
    }
}
