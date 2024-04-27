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

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "board.h"

/* System clock */
#if defined(USB_ENABLED)
/* If using USB, clock tree must be configured to generate a 48MHz clock for the
	 USB controller.  This limits the maximum CPU speed to 96 MHz */
#define PLL_M LL_RCC_PLLM_DIV_4
#define PLL_N (96)
#define PLL_P LL_RCC_PLLP_DIV_2
#define PLL_Q LL_RCC_PLLQ_DIV_8
#define CORE_CLOCK_SPEED 96000000
#define APB1_BUS_SPEED 48000000
#define APB2_BUS_SPEED 96000000
#else
/* Max out the clock, on this board that is 100MHz if we're not using USB */
#define PLL_M (LL_RCC_PLLM_DIV_4)
#define PLL_N (100)
#define PLL_P (LL_RCC_PLLP_DIV_2)
#define PLL_Q (LL_RCC_PLLQ_DIV_4)
#define CORE_CLOCK_SPEED (100000000)
#define APB1_BUS_SPEED (50000000)
#define APB2_BUS_SPEED (100000000)
#endif

/* I2C comms for Audio DAC */
#define I2C_ERR_IRQ_PRIO (0x04)
#define I2C_ERR_IRQ (I2C1_ER_IRQn)
#define I2C (I2C1)



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
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);

	/* USR BUTTON */
	LL_GPIO_SetPinMode(USR_BTN_PORT, USR_BTN_PIN, LL_GPIO_MODE_INPUT);

	/* LEDs */
	LL_GPIO_SetPinMode(LED_PORT, LED_BLUE_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(LED_PORT, LED_RED_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(LED_PORT, LED_GREEN_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(LED_PORT, LED_ORANGE_PIN, LL_GPIO_MODE_OUTPUT);

	/* MIDI */
	LL_GPIO_SetPinMode(MIDI_PORT, MIDI_TX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(MIDI_PORT, MIDI_TX_PIN, MIDI_AF);
	LL_GPIO_SetPinSpeed(MIDI_PORT, MIDI_TX_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinMode(MIDI_PORT, MIDI_RX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(MIDI_PORT, MIDI_RX_PIN, MIDI_AF);
	LL_GPIO_SetPinSpeed(MIDI_PORT, MIDI_RX_PIN, LL_GPIO_SPEED_FREQ_LOW);

	/* I2S Word Select */
	LL_GPIO_SetPinMode(I2S_WS_PORT, I2S_WS_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2S_WS_PORT, I2S_WS_PIN, I2S_AF);
	LL_GPIO_SetPinSpeed(I2S_WS_PORT, I2S_WS_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_WS_PORT, I2S_WS_PIN, LL_GPIO_PULL_DOWN);

	/* I2S Bitclock */
	LL_GPIO_SetPinMode(I2S_SCK_PORT, I2S_SCK_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_SCK_PORT, I2S_SCK_PIN, I2S_AF);
	LL_GPIO_SetPinSpeed(I2S_SCK_PORT, I2S_SCK_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_SCK_PORT, I2S_SCK_PIN, LL_GPIO_PULL_DOWN);

	/* I2S Data */
	LL_GPIO_SetPinMode(I2S_SDO_PORT, I2S_SDO_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(I2S_SDO_PORT, I2S_SDO_PIN, I2S_AF);
	LL_GPIO_SetPinSpeed(I2S_SDO_PORT, I2S_SDO_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_SDO_PORT, I2S_SDO_PIN, LL_GPIO_PULL_DOWN);

	/* I2S Master clock */
	LL_GPIO_SetPinMode(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2S_MCK_PORT, I2S_MCK_PIN, I2S_AF);
	LL_GPIO_SetPinSpeed(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinPull(I2S_MCK_PORT, I2S_MCK_PIN, LL_GPIO_PULL_DOWN);

	/* I2C SCL */
	LL_GPIO_SetPinMode(I2C_PORT, I2C_SCL_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2C_PORT, I2C_SCL_PIN, I2C_AF);
	LL_GPIO_SetPinSpeed(I2C_PORT, I2C_SCL_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(I2C_PORT, I2C_SCL_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(I2C_PORT, I2C_SCL_PIN, LL_GPIO_PULL_UP);

	/* I2C SDA */
	LL_GPIO_SetPinMode(I2C_PORT, I2C_SDA_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(I2C_PORT, I2C_SDA_PIN, I2C_AF);
	LL_GPIO_SetPinSpeed(I2C_PORT, I2C_SDA_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(I2C_PORT, I2C_SDA_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(I2C_PORT, I2C_SDA_PIN, LL_GPIO_PULL_UP);

	/* DAC Reset Pin */
	LL_GPIO_SetPinMode(DAC_PORT, DAC_RESET_PIN, LL_GPIO_MODE_OUTPUT);
}

/**
 * @brief Initialises the I2C1 channel used to configure the onboard DAC
 *
 */
static void init_i2c1_ll(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB | LL_AHB1_GRP1_PERIPH_GPIOE);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/* Disable for config */
	LL_I2C_Disable(I2C);
	while (LL_I2C_IsEnabled(I2C))
		;

	/* Set frequency */
	LL_I2C_ConfigSpeed(I2C, APB1_BUS_SPEED, LL_I2C_MAX_SPEED_FAST, LL_I2C_DUTYCYCLE_2);

	/* Set error interrupts */
	NVIC_SetPriority(I2C_ERR_IRQ, I2C_ERR_IRQ_PRIO);
	NVIC_EnableIRQ(I2C_ERR_IRQ);

	LL_I2C_Enable(I2C);
	while (!LL_I2C_IsEnabled(I2C))
		;

	/* Enable error interrupts */
	LL_I2C_EnableIT_ERR(I2C);
}




#define WAIT_FOR_SET(__REG__, __BIT__)   \
  while ((__REG__ & __BIT__) != __BIT__) \
  {                                      \
  };
#define WAIT_FOR_CLEAR(__REG__, __BIT__) \
  while (__REG__ & __BIT__)              \
  {                                      \
  };

#define EXTI0_IRQ_PRIO 0x0F
#define I2C1_ERR_IRQ_PRIO 0x04
#define APB1_SPEED_MHZ 50
#define APB2_SPEED_MHZ 100

static void init_i2c1()
{
	 uint32_t reg;

  /* Enable peripheral clocks */
  RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN);
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  /* GPIOB PB6 & PB9, Output Type AF, Open Drain, AF4 */
  GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER9);
  GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER9_1);
  GPIOB->OTYPER |= (GPIO_OTYPER_OT6 | GPIO_OTYPER_OT9);

  GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6);
  GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL6_2);

  GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL9);
  GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL9_2);

  /* Reset Codec active (PD6) */
  GPIOD->MODER &= ~GPIO_MODER_MODE4;
  GPIOD->MODER |= GPIO_MODER_MODE4_0;
  GPIOD->BSRR |= GPIO_BSRR_BS4;

  /* Reset I2C Peripheral */
  I2C1->CR1 = I2C_CR1_SWRST;
  I2C1->CR1 = 0;
  I2C1->CR2 |= I2C_CR2_ITERREN;

  /* Set frequency parameters Standard Mode (SM) 100 KHz */
  reg = I2C1->CR2 & ~I2C_CR2_FREQ;
  I2C1->CR2 = reg | APB1_SPEED_MHZ;
  reg = I2C1->CCR & ~I2C_CCR_CCR;
  I2C1->CCR = reg | (APB1_SPEED_MHZ * 5);
  reg = I2C1->TRISE & ~I2C_TRISE_TRISE;
  I2C1->TRISE = reg | (APB1_SPEED_MHZ + 1);

  /* Enable error interrupts */
  NVIC_SetPriority(I2C1_ER_IRQn, I2C1_ERR_IRQ_PRIO);
  NVIC_EnableIRQ(I2C1_ER_IRQn);

  /* Enable peripheral */
  I2C1->CR1 |= I2C_CR1_PE;
  WAIT_FOR_SET(I2C1->CR1, I2C_CR1_PE);
}

void i2c_write(uint8_t device_addr, uint8_t addr, uint8_t data)
{
	 /* Start */
  I2C1->CR1 |= I2C_CR1_START;
  WAIT_FOR_SET(I2C1->SR1, I2C_SR1_SB);

  /* Send device address for write */
  I2C1->DR = device_addr;
  WAIT_FOR_SET(I2C1->SR1, I2C_SR1_ADDR);
  /* discard response */
  I2C1->SR2;

  /* Send mem addr (register) */
  I2C1->DR = addr;
  WAIT_FOR_SET(I2C1->SR1, I2C_SR1_BTF);

  /* Send data (parameter) */
  I2C1->DR = data;
  WAIT_FOR_SET(I2C1->SR1, I2C_SR1_BTF);

  /* Stop */
  I2C1->CR1 |= I2C_CR1_STOP;
  WAIT_FOR_CLEAR(I2C1->SR2, I2C_SR2_BUSY);
}

/**
 * @brief Uses I2C1 to send configuration commands to the CS43L22 DAC
 * 
 * @param device The I2C address
 * @param reg    The command register
 * @param data   The command data (parameters)
 */
static void i2c_write_ll(uint8_t device, uint8_t reg, uint8_t data)
{
	LL_I2C_GenerateStartCondition(I2C);
	while(!LL_I2C_IsActiveFlag_SB(I2C))
	 ;

	/* Send device address */
	LL_I2C_TransmitData8(I2C,device);
	while(!LL_I2C_IsActiveFlag_ADDR(I2C))
	;

	/* Discard response */
	LL_I2C_ReceiveData8(I2C);
	
	/* Send register */
	LL_I2C_TransmitData8(I2C,reg);
	while(!LL_I2C_IsActiveFlag_BTF(I2C))
	;

	/* send data */
	LL_I2C_TransmitData8(I2C,data);
	while(!LL_I2C_IsActiveFlag_BTF(I2C))
	;

	LL_I2C_GenerateStartCondition(I2C);
	while(LL_I2C_IsActiveFlag_SB(I2C))
	;
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

	DAC_RESET();

	/* i2c1 to configure DAC, write only */
	init_i2c1();

	/* FPU ON */
	SCB->CPACR |= (3UL << 20 | 3UL << 22);

	/* ART : Prefetch, ICache, DCache on. */
	LL_FLASH_EnableDataCache();
	LL_FLASH_EnableInstCache();
	LL_FLASH_EnablePrefetch();
}
