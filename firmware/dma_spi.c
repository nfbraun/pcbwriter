#include "dma_spi.h"

#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/stm32/f4/dma.h>
#include <libopencm3/stm32/f4/spi.h>
#include <libopencm3/cm3/nvic.h>

uint8_t dma_data[10000] __attribute__((aligned(4)));
const unsigned int scanline_len = 8200; //9500;

void dma_setup(void)
{
    /* Setup data (FIXME: move elsewhere) */
    unsigned int i;
    for(i=0; i<scanline_len; i++) {
        dma_data[i] = 0x00;
    }
    dma_data[scanline_len-1] = 0xFF;
    
    /* Enable peripheral clocks. */
    rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_IOPEEN);
    rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_TIM8EN | RCC_APB2ENR_SYSCFGEN);
    
    /* Enable GPIO clocks. */
    rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPBEN | RCC_AHB1ENR_IOPDEN);
    
    /* SCK (Not needed for our application.) */
    /* gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO13);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO13); */
    
    /* MOSI */
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO15);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO15);
    
    dma_disable_stream(DMA1, DMA_STREAM4);
    
    /* Without this the interrupt routine will never be called. */
    nvic_enable_irq(NVIC_DMA1_STREAM4_IRQ);
    nvic_set_priority(NVIC_DMA1_STREAM4_IRQ, 1);
}

void start_dma(void)
{
    dma_channel_select(DMA1, DMA_STREAM4, DMA_SxCR_CHSEL_0);
    dma_set_priority(DMA1, DMA_STREAM4, DMA_SxCR_PL_VERY_HIGH);
    dma_set_peripheral_size(DMA1, DMA_STREAM4, DMA_SxCR_PSIZE_8BIT);
    dma_set_memory_size(DMA1, DMA_STREAM4, DMA_SxCR_MSIZE_8BIT);
    dma_enable_memory_increment_mode(DMA1, DMA_STREAM4);
    dma_set_transfer_mode(DMA1, DMA_STREAM4, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
    dma_enable_transfer_complete_interrupt(DMA1, DMA_STREAM4);
    
    dma_set_number_of_data(DMA1, DMA_STREAM4, scanline_len);
    dma_set_peripheral_address(DMA1, DMA_STREAM4, (uint32_t)&SPI2_DR);
    dma_set_memory_address(DMA1, DMA_STREAM4, (uint32_t)dma_data);
    
    dma_enable_stream(DMA1, DMA_STREAM4);
    
    // Start SPI DMA
    spi_enable_tx_dma(SPI2);
}

void spi_setup(void)
{
    rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);
    
    spi_reset(SPI2);
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT,
                    SPI_CR1_LSBFIRST);
    spi_enable_software_slave_management(SPI2);
    spi_set_nss_high(SPI2);
    
    spi_enable(SPI2);
}

volatile int dma_done = 0;

void dma1_stream4_isr(void)
{
    dma_clear_interrupt_flags(DMA1, DMA_STREAM4, DMA_ISR_TCIF);
    dma_disable_stream(DMA1, DMA_STREAM4);
    
    dma_done = 1;
    
    gpio_set(GPIOE, GPIO0);
    gpio_clear(GPIOE, GPIO0);
}
