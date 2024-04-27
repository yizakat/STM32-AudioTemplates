/**
 * @file board.c
 * @author yizakat (yizakat@yizakat.com)
 * @brief Board support functions, this is only stuff I need for audio projects.
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "board.h"

/* System clock */

#if defined(USB_ENABLED)
/* If using USB, clock tree must be configured to generate a 48MHz clock for the
	 USB controller.  This limits the maximum CPU speed to 96 MHz */
#define PLL_M LL_RCC_PLLM_DIV_25
#define PLL_N (192)
#define PLL_P LL_RCC_PLLP_DIV_2
#define PLL_Q LL_RCC_PLLQ_DIV_8
#define CORE_CLOCK_SPEED 96000000
#else
/* Max out the clock, on this board that is 100MHz if we're not using USB */
#define PLL_M LL_RCC_PLLM_DIV_12
#define PLL_N (96)
#define PLL_P LL_RCC_PLLP_DIV_2
#define PLL_Q LL_RCC_PLLQ_DIV_4
#define CORE_CLOCK_SPEED 100000000
#endif

/* System Clock Frequency */
uint32_t SystemCoreClock;

/**
 * @brief Updates the system core clock value
 * @details This can be hard-coded since we don't dynamically change this ever.
 * @param none
 * @retval none
 */
void SystemCoreClockUpdate(void)
{
	SystemCoreClock = CORE_CLOCK_SPEED;
}

/**
 * @brief Initialises the clock tree
 * @param none
 * @retval none
 */
static void clock_init(void)
{
	/* Switch to HSE */
	LL_RCC_HSE_Enable();
	while (!LL_RCC_HSE_IsReady())
		;

	/* Voltage output scaling for 100MHz*/
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE3);

	/* PLL OFF for configuration */
	LL_RCC_PLL_Disable();
	while (LL_RCC_PLL_IsReady())
		;

	/* Set PLL dividers and source to HSE */
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, PLL_M, PLL_N, PLL_P);

#if defined(USB_ENABLED)
	LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, PLL_M, PLL_N, PLL_Q);
#endif

	/* PLL on and wait for lock */
	LL_RCC_PLL_Enable();
	while (!LL_RCC_PLL_IsReady())
		;

	/* Switch to PLL */
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Bus clock dividers */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_SYSCLK_DIV_2);
	LL_RCC_SetAPB2Prescaler(LL_RCC_SYSCLK_DIV_1);
}

/**
 * @brief Initialises GPIO
 * @param none
 * @retval none
 */
static void gpio_init(void)
{
	/* GPIO Clocks */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

	/* MIDI */
	LL_GPIO_SetPinMode(MIDI_PORT, MIDI_TX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(MIDI_PORT, MIDI_TX_PIN, MIDI_AF);
	LL_GPIO_SetPinSpeed(MIDI_PORT, MIDI_TX_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinMode(MIDI_PORT, MIDI_RX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(MIDI_PORT, MIDI_RX_PIN, MIDI_AF);
	LL_GPIO_SetPinSpeed(MIDI_PORT, MIDI_RX_PIN, LL_GPIO_SPEED_FREQ_LOW);

	/* LED */
	LL_GPIO_SetPinMode(LED_PORT, LED_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(LED_PORT, LED_PIN, LL_GPIO_SPEED_FREQ_LOW);

	/* BTN */
	LL_GPIO_SetPinMode(USR_BTN_PORT, USR_BTN_PIN, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinSpeed(USR_BTN_PORT, USR_BTN_PIN, LL_GPIO_SPEED_FREQ_LOW);

		/* Word Select */
	LL_GPIO_SetPinMode(I2S_PORT, I2S_WS_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_PORT, I2S_WS_PIN, LL_GPIO_AF_5);
	LL_GPIO_SetPinSpeed(I2S_PORT, I2S_WS_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_PORT, I2S_WS_PIN, LL_GPIO_PULL_DOWN);

	/* Bitclock */
	LL_GPIO_SetPinMode(I2S_PORT, I2S_SCK_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_PORT, I2S_SCK_PIN, LL_GPIO_AF_5);
	LL_GPIO_SetPinSpeed(I2S_PORT, I2S_SCK_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_PORT, I2S_SCK_PIN, LL_GPIO_PULL_DOWN);

	/* Data */
	LL_GPIO_SetPinMode(I2S_PORT, I2S_SDO_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_PORT, I2S_SDO_PIN, LL_GPIO_AF_5);
	LL_GPIO_SetPinSpeed(I2S_PORT, I2S_SDO_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_PORT, I2S_SDO_PIN, LL_GPIO_PULL_DOWN);

	/* Master clock */
	LL_GPIO_SetPinMode(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_AF_5);
	LL_GPIO_SetPinSpeed(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_PULL_DOWN);


}

/**
 * @brief Enables the board.
 * @details This is called before main() by startup_stm32f411ceux.s
 * @param none
 * @retval none
 */
void board_init(void)
{
	/* Flash 3WS (4 Cycles) */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);

	/* Clock tree */
	clock_init();
	gpio_init();
	SystemCoreClockUpdate();

	
	/* FPU ON */
	SCB->CPACR |= (3UL << 20 | 3UL << 22);

	/* ART : Prefetch, ICache, DCache on. */
	LL_FLASH_EnableDataCache();
	LL_FLASH_EnableInstCache();
	LL_FLASH_EnablePrefetch();
}
