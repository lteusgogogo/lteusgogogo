/**
 *!
 * \file        mcu_stm32g0x0_spi.c
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SSPIL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************
 */

/*Includes ----------------------------------------------*/
#include "b_config.h"
#include "hal/inc/b_hal_spi.h"

#if (defined(STM32G0X0))

//         Register Address

#define SPI1_BASE_ADDR (0x40013000)
#define SPI2_BASE_ADDR (0x40003800)
#define SPI3_BASE_ADDR (0x40003C00)

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
    volatile uint32_t I2SPR;
} McuSpiReg_t;

#define MCU_SPI1 ((McuSpiReg_t *)SPI1_BASE_ADDR)
#define MCU_SPI2 ((McuSpiReg_t *)SPI2_BASE_ADDR)
#define MCU_SPI3 ((McuSpiReg_t *)SPI3_BASE_ADDR)

static McuSpiReg_t *SpiTable[3] = {MCU_SPI1, MCU_SPI2, MCU_SPI3};

int bMcuSpiSetSpeed(const bHalSPIIf_t *spi_if, bHalSPISpeed_t speed)
{
    uint16_t     SpeedVal = 1;
    McuSpiReg_t *pSpi     = NULL;
    if (IS_NULL(spi_if) || (speed >= B_HAL_SPI_SPEED_INVALID))
    {
        return -1;
    }
    if (speed == B_HAL_SPI_SLOW)
    {
        SpeedVal = 6;
    }
    if (spi_if->_if.spi > B_HAL_SPI_3)
    {
        return -1;
    }
    pSpi = SpiTable[spi_if->_if.spi];
    while (pSpi->SR & 0x80)
    {
        ;
    }
    pSpi->CR1 &= ~(0x7 << 3);
    pSpi->CR1 |= (SpeedVal << 3);
    return 0;
}

uint8_t bMcuSpiTransfer(const bHalSPIIf_t *spi_if, uint8_t dat)
{
    uint8_t      tmp  = dat;
    int          i    = 0;
    McuSpiReg_t *pSpi = NULL;

    if (IS_NULL(spi_if))
    {
        return 0;
    }

    if (spi_if->_if.spi > B_HAL_SPI_3)
    {
        return 0;
    }
    pSpi = SpiTable[spi_if->_if.spi];

    B_SET_BIT(pSpi->CR2, (0x1 << 12));

    /* Check if the SPI is already enabled */
    if ((pSpi->CR1 & (0x1 << 6)) == 0)
    {
        /* Enable SPI peripheral */
        B_SET_BIT(pSpi->CR1, 0x1 << 6);
    }

    while (B_READ_BIT(pSpi->SR, (0x1 << 1)) == 0)
    {
        ;
    }
    *(volatile uint8_t *)&pSpi->DR = dat;
    while (B_READ_BIT(pSpi->SR, (0x1 << 0)) == 0)
    {
        ;
    }
    tmp = *(volatile uint8_t *)&pSpi->DR;
    for (i = 0; i < 8; i++)
    {
        ;
    }
    return tmp;
}

int bMcuSpiSend(const bHalSPIIf_t *spi_if, const uint8_t *pbuf, uint16_t len)
{
    int i = 0;
    if (IS_NULL(spi_if) || IS_NULL(pbuf))
    {
        return -1;
    }
    for (i = 0; i < len; i++)
    {
        bMcuSpiTransfer(spi_if, pbuf[i]);
    }
    return 0;
}

int bMcuSpiReceive(const bHalSPIIf_t *spi_if, uint8_t *pbuf, uint16_t len)
{
    int i = 0;
    if (IS_NULL(spi_if) || IS_NULL(pbuf))
    {
        return -1;
    }
    for (i = 0; i < len; i++)
    {
        pbuf[i] = bMcuSpiTransfer(spi_if, 0xff);
    }
    return 0;
}

#endif

/************************ Copyright (c) 2020 Bean *****END OF FILE****/
