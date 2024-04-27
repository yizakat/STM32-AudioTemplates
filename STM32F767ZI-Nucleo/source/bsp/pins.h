/**
 * @file pins.h
 * @author yizakat (yizakat@yizakat.com)
 * @brief  STM32F767ZI Pins
 * @version 0.1
 * @date 2024-01-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* Macros for logic analyser */
#define PROBE1_SET() (GPIOD->BSRR |= GPIO_BSRR_BS0)   
#define PROBE2_SET() (GPIOD->BSRR |= GPIO_BSRR_BS1)   
#define PROBE3_SET() (GPIOD->BSRR |= GPIO_BSRR_BS2)   
#define PROBE4_SET() (GPIOD->BSRR |= GPIO_BSRR_BS7)   
#define PROBE5_SET() (GPIOD->BSRR |= GPIO_BSRR_BS4)   
#define PROBE6_SET() (GPIOD->BSRR |= GPIO_BSRR_BS5)   
#define PROBE7_SET() (GPIOD->BSRR |= GPIO_BSRR_BS6)   
#define PROBE8_SET() (GPIOD->BSRR |= GPIO_BSRR_BS7)   

#define PROBE1_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR0)
#define PROBE2_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR1)
#define PROBE3_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR2)
#define PROBE4_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR7)
#define PROBE5_CLEAR() (GPIOC->BSRR |= GPIO_BSRR_BR4)
#define PROBE6_CLEAR() (GPIOC->BSRR |= GPIO_BSRR_BR5)
#define PROBE7_CLEAR() (GPIOE->BSRR |= GPIO_BSRR_BR7)
#define PROBE8_CLEAR() (GPIOE->BSRR |= GPIO_BSRR_BR9)

/* Probes */
#define PROBE1_PIN  (LL_GPIO_PIN_0) /* D0 */
#define PROBE1_PORT (GPIOB)
#define PROBE2_PIN  (LL_GPIO_PIN_1) /* D1 */
#define PROBE2_PORT (GPIOB)
#define PROBE3_PIN  (LL_GPIO_PIN_2) /* D2 */
#define PROBE3_PORT (GPIOB)
#define PROBE4_PIN  (LL_GPIO_PIN_7) /* D3 */
#define PROBE4_PORT (GPIOB)
#define PROBE5_PIN  (LL_GPIO_PIN_4) /* D4 */
#define PROBE5_PORT (GPIOC)
#define PROBE6_PIN  (LL_GPIO_PIN_5) /* D5 */
#define PROBE6_PORT (GPIOC)
#define PROBE7_PIN  (LL_GPIO_PIN_7) /* D6 */
#define PROBE7_PORT (GPIOE)
#define PROBE8_PIN  (LL_GPIO_PIN_9) /* D7 */
#define PROBE8_PORT (GPIOE)


/* Macros for quick LED/BTN access */
#define LED_GREEN_ON() (GPIOB->BSRR |= GPIO_BSRR_BS0)
#define LED_BLUE_ON() (GPIOB->BSRR |= GPIO_BSRR_BS7)
#define LED_RED_ON() (GPIOB->BSRR |= GPIO_BSRR_BS14)


#define LED_GREEN_OFF() (GPIOB->BSRR |= GPIO_BSRR_BR0)
#define LED_BLUE_OFF() (GPIOB->BSRR |= GPIO_BSRR_BR7)
#define LED_RED_OFF() (GPIOB->BSRR |= GPIO_BSRR_BR14)


/* LEDS */
#define LED_GREEN_PIN (LL_GPIO_PIN_0) /* B0 */
#define LED_GREEN_PORT (GPIOB)
#define LED_BLUE_PIN (LL_GPIO_PIN_7)  /* B7 */
#define LED_BLUE_PORT (GPIOB)
#define LED_RED_PIN (LL_GPIO_PIN_14)  /* B14 */
#define LED_RED_PORT (GPIOB)

/* USER Button */
#define USR_BTN_PIN (LL_GPIO_PIN_13)  /* C13 */
#define USR_BTN_PORT (GPIOC)
#define USR_BTN_READ() ((GPIOC->IDR & (1 << USR_BTN_PIN)) >> USR_BTN_PIN

/* MIDI (UART1) */
#define MIDI_TX_PIN (LL_GPIO_PIN_6)  /* B6 */
#define MIDI_RX_PIN (LL_GPIO_PIN_15) /* B15 */
#define MIDI_AF (LL_GPIO_AF_7)       /* AF7 */
#define MIDI_PORT (GPIOB)

/* I2S3 */


/* I2S */

#define I2S_WS_PIN (LL_GPIO_PIN_4) /* A4 */
#define I2S_WS_AF (LL_GPIO_AF_6)   /* AF6 */
#define I2S_WS_PORT (GPIOA)

#define I2S_SCK_PIN (LL_GPIO_PIN_10) /* C10 */
#define I2S_SCK_AF (LL_GPIO_AF_6)   /* AF6 */
#define I2S_SCK_PORT (GPIOC)

#define I2S_SDO_PIN (LL_GPIO_PIN_12) /* C12 */
#define I2S_SDO_AF (LL_GPIO_AF_6)   /* AF6 */
#define I2S_SDO_PORT (GPIOC)

#define I2S_MCK_PIN (LL_GPIO_PIN_7) /* C7 */
#define I2S_MCK_AF (LL_GPIO_AF_6)   /* AF6 */
#define I2S_MCK_PORT (GPIOC)    


