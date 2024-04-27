/**
 * @file audio.c
 * @author yizakat (yizakat@yizakat.com)
 * @brief  Enables audio streaming via I2S2 and DMA1
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "audio.h"

/*
 * This is a LUT of the various config items for specific audio modes that we support.
 */
audio_config_t configs[] =
		{
#if SAMPLE_RESOLUTION == 16			
				{.N = 302, .R = 2, .DIV = 53, .ODD = 1, .MCKOE = 0, .bits = 16, .type = I2S_44_16, .fsr = 44100.46875f},
				{.N = 192, .R = 5, .DIV = 12, .ODD = 1, .MCKOE = 0, .bits = 16, .type = I2S_48_16, .fsr = 48000.0f},
				{.N = 290, .R = 2, .DIV = 6, .ODD = 0, .MCKOE = 1, .bits = 16, .type = I2S_44_MCKOE_16, .fsr = 43569.0f},
				{.N = 271, .R = 2, .DIV = 6, .ODD = 0, .MCKOE = 1, .bits = 16, .type = I2S_48_MCKOE_16, .fsr = 47991.07031},
#else
				{.N = 429, .R = 4, .DIV = 19, .ODD = 0, .MCKOE = 0, .bits = 32, .type = I2S_44_32, .fsr = 44099.50781f},
				{.N = 384, .R = 5, .DIV = 12, .ODD = 1, .MCKOE = 0, .bits = 32, .type = I2S_48_32, .fsr = 48000.0f},
				{.N = 290, .R = 2, .DIV = 6, .ODD = 0, .MCKOE = 1, .bits = 32, .type = I2S_44_MCKOE_32, .fsr = 43569.0f},
				{.N = 258, .R = 3, .DIV = 3, .ODD = 1, .MCKOE = 1, .bits = 32, .type = I2S_48_MCKOE_32, .fsr = 47991.07031}
#endif
};

/**
 * @brief Enables I2S audio stream using DMA circular buffering.
 *
 * @param audio_buffer The audio buffer
 * @param audio_mode The audio mode (this determines I2S clock configuration)
 * @return audio_config_t*  The config from LUT (the caller needs this)
 */
audio_config_t *audio_streaming_run(int16_t audio_buffer[], audio_mode_t audio_mode)
{
	/* Lookup the configuration details for the requested mode */
	audio_config_t *config = &configs[audio_mode];

	/* Peripheral clocks on */
	LL_AHB1_GRP1_EnableClock(I2S_DMA_CLK);
	LL_APB1_GRP1_EnableClock(I2S_SPI_CLK);

	/* I2S PLL off for configuration */
	LL_RCC_PLLI2S_Disable();
	while (LL_RCC_PLLI2S_IsReady())
		;

	/* I2S off for to configuration */
	LL_I2S_Disable(I2S_SPI);
	while (LL_I2S_IsEnabled(I2S_SPI))
		;

	/* DMA off for configuration*/
	LL_DMA_DisableStream(I2S_DMA, I2S_DMA_STREAM);
	while (LL_DMA_IsEnabledStream(I2S_DMA, I2S_DMA_STREAM))
		;

	/* I2S standard config */
	LL_I2S_SetStandard(I2S_SPI, LL_I2S_STANDARD_PHILIPS);
	LL_I2S_SetTransferMode(I2S_SPI, LL_I2S_MODE_MASTER_TX);
	LL_I2S_SetClockPolarity(I2S_SPI, LL_I2S_POLARITY_LOW);

	/* Must be using this wrong, reverted to registers to set dividers */
	/* LL_RCC_PLLI2S_ConfigDomain_I2S(LL_RCC_PLLSOURCE_HSE,I2S_M, config->N, config->R); */

	/* Set I2S dividers */
	MODIFY_REG(RCC->PLLI2SCFGR,
			RCC_PLLI2SCFGR_PLLI2SM | RCC_PLLI2SCFGR_PLLI2SN | RCC_PLLI2SCFGR_PLLI2SR,
			_VAL2FLD(RCC_PLLI2SCFGR_PLLI2SM, I2S_M) |
			_VAL2FLD(RCC_PLLI2SCFGR_PLLI2SN, config->N) |
			_VAL2FLD(RCC_PLLI2SCFGR_PLLI2SR, config->R));


	/* I2S configuration for requested mode */	
	LL_I2S_SetDataFormat(I2S_SPI, config->bits == 16 ? LL_I2S_DATAFORMAT_16B : LL_I2S_DATAFORMAT_24B);

	config->MCKOE ? LL_I2S_EnableMasterClock(I2S_SPI) : LL_I2S_DisableMasterClock(I2S_SPI);
	LL_I2S_SetPrescalerLinear(I2S_SPI, config->DIV);
	LL_I2S_SetPrescalerParity(I2S_SPI, config->ODD ? LL_I2S_PRESCALER_PARITY_ODD : LL_I2S_PRESCALER_PARITY_EVEN);

	LL_I2S_Enable(I2S_SPI);
	
	/* DMA Source and target addresses */
	LL_DMA_ConfigAddresses(I2S_DMA, I2S_DMA_STREAM, (uint32_t)audio_buffer, LL_SPI_DMA_GetRegAddr(I2S_SPI), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	/* DMA transfer length */
	LL_DMA_SetDataLength(I2S_DMA, I2S_DMA_STREAM,AUDIO_BUF_DBL);

	/* DMA controller configuration */
	LL_DMA_ConfigTransfer(I2S_DMA, I2S_DMA_STREAM,
												LL_DMA_PRIORITY_HIGH |
														LL_DMA_MDATAALIGN_HALFWORD |
														LL_DMA_PDATAALIGN_HALFWORD |
														LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
														LL_DMA_FIFOMODE_ENABLE |
														LL_DMA_FIFOTHRESHOLD_FULL |
														LL_DMA_MEMORY_INCREMENT |
														LL_DMA_PERIPH_NOINCREMENT |
														LL_DMA_MODE_CIRCULAR);

	/* Request that we're sent interrupts */
	LL_DMA_EnableIT_TC(I2S_DMA, I2S_DMA_STREAM); /* Transfer complete interrupt */
	LL_DMA_EnableIT_HT(I2S_DMA, I2S_DMA_STREAM); /* Transfer half complete interrupt */
	LL_DMA_EnableIT_TE(I2S_DMA, I2S_DMA_STREAM); /* Transfer error interrupt */

	/* DMA Tx Enable */
	LL_SPI_EnableDMAReq_TX(I2S_SPI);

	/* Enable DMA interrupt */
	NVIC_EnableIRQ(I2S_DMA_IRQ);

	/* I2S PLL on */
	LL_RCC_PLLI2S_Enable();
	while (!LL_RCC_PLLI2S_IsReady())
		;

	/* I2S on */
	LL_I2S_Enable(I2S_SPI);
	while (!LL_I2S_IsEnabled(I2S_SPI))
		;

	/* DMA on */
	LL_DMA_EnableStream(I2S_DMA, I2S_DMA_STREAM);
	while (!LL_DMA_IsEnabledStream(I2S_DMA, I2S_DMA_STREAM))
		;

	return config;
}

