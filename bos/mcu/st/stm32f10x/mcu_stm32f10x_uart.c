/**
 *!
 * \file        mcu_stm32f10x_uart.c
 * \version     v0.0.1
 * \date        2020/03/25
 * \author      Bean(notrynohigh@outlook.com)
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2020 Bean
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SUARTL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************
 */

/*Includes ----------------------------------------------*/
#include "b_config.h"
#include "hal/inc/b_hal_uart.h"

#if (defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_HD) || \
     defined(STM32F10X_CL))

//      Register Address

#define UART1_BASE_ADDR (0x40013800)
#define UART2_BASE_ADDR (0x40004400)
#define UART3_BASE_ADDR (0x40004800)
#define UART4_BASE_ADDR (0x40004C00)
#define UART5_BASE_ADDR (0x40005000)

typedef struct
{
    volatile uint16_t SR;
    uint16_t          RESERVED0;
    volatile uint16_t DR;
    uint16_t          RESERVED1;
    volatile uint16_t BRR;
    uint16_t          RESERVED2;
    volatile uint16_t CR1;
    uint16_t          RESERVED3;
    volatile uint16_t CR2;
    uint16_t          RESERVED4;
    volatile uint16_t CR3;
    uint16_t          RESERVED5;
    volatile uint16_t GTPR;
    uint16_t          RESERVED6;
} McuUartReg_t;

#define MCU_UART1 ((McuUartReg_t *)UART1_BASE_ADDR)
#define MCU_UART2 ((McuUartReg_t *)UART2_BASE_ADDR)
#define MCU_UART3 ((McuUartReg_t *)UART3_BASE_ADDR)
#define MCU_UART4 ((McuUartReg_t *)UART4_BASE_ADDR)
#define MCU_UART5 ((McuUartReg_t *)UART5_BASE_ADDR)

static McuUartReg_t *UartTable[5] = {MCU_UART1, MCU_UART2, MCU_UART3, MCU_UART4, MCU_UART5};

int bMcuUartSend(bHalUartNumber_t uart, const uint8_t *pbuf, uint16_t len)
{
    int           i       = 0;
    int           timeout = 0x000B0000;
    McuUartReg_t *pUart   = NULL;
    if (uart > B_HAL_UART_5 || pbuf == NULL)
    {
        return -1;
    }
    pUart = UartTable[uart];
    for (i = 0; i < len; i++)
    {
        timeout = 0x000B0000;
        while (timeout > 0 && ((pUart->SR & (0x1 << 6)) == 0))
        {
            timeout--;
        }
        if (timeout <= 0)
        {
            return -2;
        }
        pUart->DR = pbuf[i];
    }
    return len;
}

int bMcuReceive(bHalUartNumber_t uart, uint8_t *pbuf, uint16_t len)
{
    int           i       = 0;
    int           timeout = 0x000B0000;
    McuUartReg_t *pUart   = NULL;
    if (uart > B_HAL_UART_5 || pbuf == NULL)
    {
        return -1;
    }
    pUart = UartTable[uart];
    for (i = 0; i < len; i++)
    {
        timeout = 0x000B0000;
        while (timeout > 0 && ((pUart->SR & (0x1 << 5)) == 0))
        {
            timeout--;
        }
        if (timeout <= 0)
        {
            return -2;
        }
        pbuf[i] = pUart->DR;
    }
    return len;
}

#endif

/************************ Copyright (c) 2020 Bean *****END OF FILE****/
