/**
 * @file audio.c
 * @author yizakat (yizakat@yizakat.com)
 * @brief Enables audio streaming via I2S and DMA
 * @version 0.1
 * @date 2024-01-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "audio.h"

/*
 * This is a LUT of the various config items for specific audio modes.  The could be calculated
 * on the fly but I prefer to precalc and use a LUT.
 * 
 * This board uses the on-board Audio DAC on I2S3 with a hard-wired master clock.
 * 
 * PLLI2S_IN = HSE_VALUE/PLL_M = 8/1 = 1MHz 
 * 
 * For FSR multiples of 8 (eg 48,192) : PLLI2S_OUT = PLLI2S_IN * N = 1 * 258 = 258MHz
 * 																			I2SCLK = PLLI2S_OUT/R =  258/3 = 86MHz
 * 
 * For others (eg 44.1)								: PLLI2S_OUT = PLLI2S_IN * N = 1 * 290 = 290MHz
 *                                      I2SCLK = PLLI2S_OUT/R  = 290/2 = 145MHz 
 * 
 * The DIV factor calculation looks like this:
 * 
 *  	VAL = ((((I2SCLK / PACKET_LEN) * 10)/FSR)+5) / 10;
 * 		ODD = (VAL & 1) 
 * 		DIV = (VAL - ODD) / 2;
 * 
 * The PACKET_LEN varies depending on the WORD_LENGTH and whether the master clock is
 * enabled (MCKOE)
 * 
 * IF MCKOE (master clock enabled)
 * 		For 16 bit WORD_LENGTH: PACKET_LEN= WORD_LENGTH * 2 * 8 = 256 
 * 		For 32 bit WORD_LENGTH: PACKET_LEN= WORD_LENGTH * 2 * 4 = 256 
 * ELSE
 * 		For 16 bit WORD: PACKET_LEN = WORD_LENGTH * 2 * 10 = 320
 * 		For 32 bit WORD: PACKET_LEN = WORD_LENGTH * 2 * 10 = 640
 
 * Eg: 44100 @ 32 bits
 * 
 * 		VAL = ((((145000000 / 256) * 10)/44100)+5) / 10 = 13
 * 		ODD = (13 & 1) = 1
 * 		DIV = (13-1/2) = 6
 * 
 * Eg: 48000 @ 32 bits
 *    VAL = ((((86000000 / 256) * 10)/44100)+5) / 10 = 7
 *    ODD = 1
 *    DIV = 7-1 /2 = 3
 * 
 * NOTE: You must set the WORD_LENGTH #define in audio.h to the correct buffer size to match the
 * bits value here.
 */
audio_config_t configs[] =
{								
#if SAMPLE_RESOLUTION == 16
	{.N = 271, .R = 2, .DIV = 6, .ODD = 0, .MCKOE = 1, .bits = 32, .type = I2S_44_MCKOE_32, .fsr = 44108.0f},
	{.N = 258, .R=  3, .DIV =` 3, .ODD = 0, .MCKOE = 1, .bits = 16, .type = I2S_48_MCKOE_16, .fsr = 47991.0f},
#else
	{.N = 271, .R = 2, .DIV = 6, .ODD = 0, .MCKOE = 1, .bits = 32, .type = I2S_44_MCKOE_32, .fsr = 44108.0f},
	{.N = 258, .R=  3, .DIV = 3, .ODD = 1, .MCKOE = 1, .bits = 32, .type = I2S_48_MCKOE_32, .fsr = 47991.0f}				
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

	// /* Peripheral clocks on */
	LL_APB1_GRP1_EnableClock(I2S_SPI_CLK);
	LL_AHB1_GRP1_EnableClock(I2S_DMA_CLK);
	
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

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

		/* Set I2S dividers */
	MODIFY_REG(RCC->PLLI2SCFGR,
			RCC_PLLI2SCFGR_PLLI2SN | RCC_PLLI2SCFGR_PLLI2SR,			
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
	LL_DMA_SetDataLength(I2S_DMA, I2S_DMA_STREAM, AUDIO_BUF_DBL);

	/* DMA controller configuration */
	LL_DMA_ConfigTransfer(I2S_DMA,I2S_DMA_STREAM,
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

