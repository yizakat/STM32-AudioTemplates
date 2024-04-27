/**
 * @file pins.h
 * @author yizakat (yizakat@yizakat.com)
 * @brief  GPIO & Peripheral Pins for this BSP
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */


/* Debug probes pins for logic analyser */
#define PROBE1_SET() (GPIOB->BSRR |= GPIO_BSRR_BS0)
#define PROBE2_SET() (GPIOB->BSRR |= GPIO_BSRR_BS1)
#define PROBE3_SET() (GPIOB->BSRR |= GPIO_BSRR_BS2)
#define PROBE4_SET() (GPIOB->BSRR |= GPIO_BSRR_BS7)
#define PROBE5_SET() (GPIOC->BSRR |= GPIO_BSRR_BS4)
#define PROBE6_SET() (GPIOC->BSRR |= GPIO_BSRR_BS5)
#define PROBE7_SET() (GPIOE->BSRR |= GPIO_BSRR_BS7)
#define PROBE8_SET() (GPIOE->BSRR |= GPIO_BSRR_BS9)

#define PROBE1_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR0)
#define PROBE2_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR1)
#define PROBE3_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR2)
#define PROBE4_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR7)
#define PROBE5_CLEAR() (GPIOC->BSRR |= GPIO_BSRR_BR4)
#define PROBE6_CLEAR() (GPIOC->BSRR |= GPIO_BSRR_BR5)
#define PROBE7_CLEAR() (GPIOE->BSRR |= GPIO_BSRR_BR7)
#define PROBE8_CLEAR() (GPIOE->BSRR |= GPIO_BSRR_BR9)

/* Macros for quick LED/BTN access */
#define LED_GREEN_ON() (GPIOD->BSRR |= GPIO_BSRR_BS12)
#define LED_ORANGE_ON() (GPIOD->BSRR |= GPIO_BSRR_BS13)
#define LED_RED_ON() (GPIOD->BSRR |= GPIO_BSRR_BS14)
#define LED_BLUE_ON() (GPIOD->BSRR |= GPIO_BSRR_BS15)

#define LED_GREEN_OFF() (GPIOD->BSRR |= GPIO_BSRR_BR12)
#define LED_ORANGE_OFF() (GPIOD->BSRR |= GPIO_BSRR_BR13)
#define LED_RED_OFF() (GPIOD->BSRR |= GPIO_BSRR_BR14)
#define LED_BLUE_OFF() (GPIOD->BSRR |= GPIO_BSRR_BR15))

/* LEDS */
#define LED_GREEN_PIN (LL_GPIO_PIN_12)
#define LED_ORANGE_PIN (LL_GPIO_PIN_13)
#define LED_RED_PIN (LL_GPIO_PIN_14)
#define LED_BLUE_PIN (LL_GPIO_PIN_15)
#define LED_PORT (GPIOD)

/* USER Button */
#define USR_BTN_PIN (LL_GPIO_PIN_0)
#define USR_BTN_PORT (GPIOA)
#define USR_BTN_READ() ((GPIOA->IDR & (1 << USR_BTN_PIN)) >> USR_BTN_PIN

/* MIDI (UART1) */
#define MIDI_TX_PIN (LL_GPIO_PIN_9)  /* PA9 */
#define MIDI_RX_PIN (LL_GPIO_PIN_10) /* PA10 */
#define MIDI_AF (LL_GPIO_AF_7)       /* AF7 */
#define MIDI_PORT (GPIOA)

/* I2S */
#define I2S_AF (LL_GPIO_AF_6)      /* AF6 */

#define I2S_WS_PIN (LL_GPIO_PIN_4) /* A4 */
#define I2S_WS_PORT (GPIOA)

#define I2S_SDO_PIN (LL_GPIO_PIN_12) /* C12 */
#define I2S_SDO_PORT (GPIOC)

#define I2S_SCK_PIN (LL_GPIO_PIN_10) /* C10 */
#define I2S_SCK_PORT (GPIOC)

#define I2S_MCK_PIN (LL_GPIO_PIN_7) /* C7 */
#define I2S_MCK_PORT (GPIOC)        

/* I2C */
#define I2C_SCL_PIN (LL_GPIO_PIN_6) /* B6 */
#define I2C_SDA_PIN (LL_GPIO_PIN_9) /* B9 */
#define I2C_PORT (GPIOB)            /* GPIOB */
#define I2C_AF (LL_GPIO_AF_4)       /* AF4 */

/* DAC hardware reset */
#define DAC_RESET_PIN (LL_GPIO_PIN_6) /* D6 */
#define DAC_PORT (GPIOD)              /* GPIOD */
#define DAC_RESET() (GPIOD->BSRR |= GPIO_BSRR_BR6)