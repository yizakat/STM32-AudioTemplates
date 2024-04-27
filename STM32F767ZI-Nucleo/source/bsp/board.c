/**
 * @file board.c
 * @author yizakat (yizakat@yizakat.com)
 * @brief Board support functions
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stm32f7xx.h"
#include "system_stm32f7xx.h"
#include "board.h"

/* System clock */
#if defined(USB_ENABLED)
/* If using USB, clock tree must be configured to generate a 48MHz clock for the
	 USB controller.  This limits the maximum CPU speed to 192 MHz */
#define PLL_M LL_RCC_PLLM_DIV_4
#define PLL_N (192)
#define PLL_P LL_RCC_PLLP_DIV_2
#define PLL_Q LL_RCC_PLLQ_DIV_8
#defome PLL_R LL_RCC_PLLR_DIV_2
#define CORE_CLOCK_SPEED 192000000
#define APB1_BUS_SPEED 48000000
#define APB2_BUS_SPEED 96000000
#else
/* Max out the clock, on this board that is 216MHz */
#define PLL_M (LL_RCC_PLLM_DIV_8)
#define PLL_N (216)
#define PLL_P (LL_RCC_PLLP_DIV_2)
#define PLL_Q (LL_RCC_PLLQ_DIV_2)
#define PLL_R (LL_RCC_PLLR_DIV_2)
#define CORE_CLOCK_SPEED (21600000)
#define APB1_BUS_SPEED (54000000)
#define APB2_BUS_SPEED (108000000)
#endif


// #if defined(USB_ENABLED)
// /* If using USB, clock tree must be configured to generate a 48MHz clock for the
// 	 USB controller.  This limits the maximum CPU speed to 96 MHz */
// #define PLL_M LL_RCC_PLLM_DIV_4
// #define PLL_N (96)
// #define PLL_P LL_RCC_PLLP_DIV_2
// #define PLL_Q LL_RCC_PLLQ_DIV_8
// #define CORE_CLOCK_SPEED 96000000
// #define APB1_BUS_SPEED 48000000
// #define APB2_BUS_SPEED 96000000
// #else
// /* Max out the clock, on this board that is 100MHz if we're not using USB */
// #define PLL_M (LL_RCC_PLLM_DIV_4)
// #define PLL_N (100)
// #define PLL_P (LL_RCC_PLLP_DIV_2)
// #define PLL_Q (LL_RCC_PLLQ_DIV_4)
// #define CORE_CLOCK_SPEED (100000000)
// #define APB1_BUS_SPEED (50000000)
// #define APB2_BUS_SPEED (100000000)
// #endif


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
	/* Using HSE from 8MHz clock on onboard ST-LINK (Bypass) */
	LL_RCC_HSE_EnableBypass();
	LL_RCC_HSE_Enable();
	while (!LL_RCC_HSE_IsReady())
		;

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	/* Flash 7WS (8 Cycles) */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);

/* Activation OverDrive Mode */
  LL_PWR_EnableOverDriveMode();
  while(LL_PWR_IsActiveFlag_OD() != 1)
  {
  };
	

	/* Voltage output scaling high for 216MHz*/	
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

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
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };

	// SysTick_Config(CORE_CLOCK_SPEED / 1000);

	/* Bus clock dividers */
	LL_RCC_SetAPB1Prescaler(LL_RCC_SYSCLK_DIV_2);
	LL_RCC_SetAPB2Prescaler(LL_RCC_SYSCLK_DIV_1);

	/* Let CMSIS know */
	SystemCoreClockUpdate();
}

/**
 * @brief Initialises GPIO & AF for all peripherals
 * @param none
 * @retval none
 */
static void gpio_init(void)
{
	/* Clocks - need pretty well all of them */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

	/* USR BUTTON */
	LL_GPIO_SetPinMode(USR_BTN_PORT, USR_BTN_PIN, LL_GPIO_MODE_INPUT);

	/* LEDs */
	LL_GPIO_SetPinMode(LED_BLUE_PORT, LED_BLUE_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(LED_RED_PORT, LED_RED_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(LED_GREEN_PORT, LED_GREEN_PIN, LL_GPIO_MODE_OUTPUT);


	/* MIDI */
	LL_GPIO_SetPinMode(MIDI_PORT, MIDI_TX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(MIDI_PORT, MIDI_TX_PIN, MIDI_AF);
	LL_GPIO_SetPinSpeed(MIDI_PORT, MIDI_TX_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinMode(MIDI_PORT, MIDI_RX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(MIDI_PORT, MIDI_RX_PIN, MIDI_AF);
	LL_GPIO_SetPinSpeed(MIDI_PORT, MIDI_RX_PIN, LL_GPIO_SPEED_FREQ_LOW);

	/* I2S Word Select */
	LL_GPIO_SetPinMode(I2S_WS_PORT, I2S_WS_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2S_WS_PORT, I2S_WS_PIN, I2S_WS_AF);
	LL_GPIO_SetPinSpeed(I2S_WS_PORT, I2S_WS_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_WS_PORT, I2S_WS_PIN, LL_GPIO_PULL_DOWN);

	/* I2S Bitclock */
	LL_GPIO_SetPinMode(I2S_SCK_PORT, I2S_SCK_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_SCK_PORT, I2S_SCK_PIN, I2S_SCK_AF);
	LL_GPIO_SetPinSpeed(I2S_SCK_PORT, I2S_SCK_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_SCK_PORT, I2S_SCK_PIN, LL_GPIO_PULL_DOWN);

	/* I2S Data */
	LL_GPIO_SetPinMode(I2S_SDO_PORT, I2S_SDO_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_SDO_PORT, I2S_SDO_PIN, I2S_SDO_AF);
	LL_GPIO_SetPinSpeed(I2S_SDO_PORT, I2S_SDO_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_SDO_PORT, I2S_SDO_PIN, LL_GPIO_PULL_DOWN);

	/* I2S Master clock */
	LL_GPIO_SetPinMode(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2S_MCK_PORT, I2S_MCK_PIN, I2S_MCK_AF);
	LL_GPIO_SetPinSpeed(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_PULL_DOWN);

	/* Probe Pins */
	LL_GPIO_SetPinMode(PROBE1_PORT, PROBE1_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE2_PORT, PROBE2_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE3_PORT, PROBE3_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE4_PORT, PROBE4_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE5_PORT, PROBE5_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE6_PORT, PROBE6_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE7_PORT, PROBE7_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(PROBE8_PORT, PROBE8_PIN, LL_GPIO_MODE_OUTPUT);
}


/**
 * @brief Enable the memory protection unit
 * @details This is needed to get full performance from the MCU
 * @param none
 * @retval none
 */
void mpu_init(void)
{

	LL_MPU_Disable();

	/* Enable RAM region 0 */
	LL_MPU_ConfigRegion(LL_MPU_REGION_NUMBER0,0x87,0x0,
		LL_MPU_REGION_SIZE_4GB 
		| LL_MPU_TEX_LEVEL0 
		| LL_MPU_REGION_NO_ACCESS 
		| LL_MPU_INSTRUCTION_ACCESS_DISABLE 
		| LL_MPU_ACCESS_SHAREABLE 
		| LL_MPU_ACCESS_NOT_CACHEABLE 
		| LL_MPU_ACCESS_NOT_BUFFERABLE
	);

	LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
}

/**
 * @brief Enables the board.
 * @details This is called before main() by startup_stm32f411ceux.s
 * @param none
 * @retval none
 */
void board_init(void)
{
	/* Memory protection unit*/
	// mpu_init();

	/* Caches */
	SCB_EnableICache();
	//SCB_EnableDCache();



	/* Clock tree */
	clock_init();
	gpio_init();

	/* FPU ON */
	SCB->CPACR |= (3UL << 20 | 3UL << 22);	
}
