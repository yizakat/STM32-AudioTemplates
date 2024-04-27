/**
 * @file main.c
 * @author yizakat (yizakat@yizakat.com)
 * @brief  Template audio starter app.
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdint.h>
#include "audio.h"
#include "board.h"

static int16_t audio_buffer[AUDIO_BUF_DBL];

#define REFILL_DONE 0
#define REFILL_PING 1
#define REFILL_PONG 2
volatile static uint8_t buf_state = REFILL_DONE;

/* ----------------------------------------------------------------------------
 * Trivial test oscillators - no saw anti-aliasing and sine is only an approx.
 */
#define TEST_TONE 440.0f

static float acc = 0.5f;
static float sample_buffer[SAMPLE_BLOCK_SIZE];

static void GenerateSaw(float inc)
{
	for (int i = 0; i < SAMPLE_BLOCK_SIZE; i++)
	{
		if (acc > 1.0f)
		{
			acc -= 1.0f;
		}

		sample_buffer[i] = (2.0f * acc - 1.0f);
		acc += inc;
	}
}

const float PI = 3.1415926535897932384626433832795f;
const float B = 4.0f / PI;
const float C = -4.0f / (PI * PI);
const float P = 0.225f;

#include <math.h>

static void GenerateSineApproximation(float inc)
{
	for (int i = 0; i < SAMPLE_BLOCK_SIZE; i++)
	{
		if (acc > 1.0f)
		{
			acc -= 1.0f;
		}

		float angle = -1.0f * (acc * 2.0f * PI - PI);
		float y = B * angle + C * angle * fabsf(angle);

		sample_buffer[i] = (P * (y * fabsf(y) - y) + y);
		acc += inc;
	}
}

/* ----------------------------------------------------------------------------
 * Program entry point
 */
int main(void)
{

	audio_config_t *pConfig = audio_streaming_run(audio_buffer, I2S_48_MCKOE_32);

	/* Signal that all is well post configuration */
	LED_ON();

	while (1)
	{
		if (buf_state != REFILL_DONE)
		{
			PROBE1_SET();

			// GenerateSaw(TEST_TONE / pConfig->fsr);
			GenerateSineApproximation(TEST_TONE / pConfig->fsr);

			int16_t *ptr =
					buf_state == REFILL_PING ? audio_buffer : audio_buffer + AUDIO_BUF_SGL;

			/* Determine buffer half to refill */
			if (pConfig->bits == 16)
			{
				for (int i = 0; i < SAMPLE_BLOCK_SIZE; i++)
				{
					int16_t sample = sample_buffer[i] * INT16_MAX;

					/* LEFT + RIGHT */
					*ptr++ = sample;
					*ptr++ = sample;
				}
			}
			else
			{
				for (int i = 0; i < SAMPLE_BLOCK_SIZE; i++)
				{
					int32_t sample = sample_buffer[i] * INT32_MAX;

					/* LEFT + RIGHT */
					*ptr++ = (int16_t)(sample >> 16); /* MSB in first word */
					*ptr++ = (int16_t)(sample);				/* LSB in 2nd word */
					*ptr++ = (int16_t)(sample >> 16);
					*ptr++ = (int16_t)(sample);
				}
			}
			buf_state = REFILL_DONE;

			PROBE1_CLEAR();
		}
	}
}

/* -------------------------------------------------------------------
 * DMA1_4 interrupts for I2S2 TX (SPI2) transfers
 */
void DMA1_Stream4_IRQHandler(void)
{
	if (LL_DMA_IsActiveFlag_TC4(I2S_DMA) == 1)
	{
		/* Complete */
		LL_DMA_ClearFlag_TC4(I2S_DMA);
		buf_state = REFILL_PONG;
	}
	else if (LL_DMA_IsActiveFlag_HT4(I2S_DMA) == 1)
	{
		/* Half complete */
		LL_DMA_ClearFlag_HT4(I2S_DMA);
		buf_state = REFILL_PING;
	}
	else if (LL_DMA_IsActiveFlag_TE4(I2S_DMA) == 1)
	{
		/* Oopsie */
		LL_DMA_ClearFlag_TE4(I2S_DMA);
	}
}
