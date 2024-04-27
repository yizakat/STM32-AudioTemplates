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

/* Pins B1-B8 are used for general purpose logic analyser probes */
#define PROBE1_SET() (GPIOB->BSRR |= GPIO_BSRR_BS1)
#define PROBE2_SET() (GPIOB->BSRR |= GPIO_BSRR_BS2)
#define PROBE3_SET() (GPIOB->BSRR |= GPIO_BSRR_BS3)
#define PROBE4_SET() (GPIOB->BSRR |= GPIO_BSRR_BS4)
#define PROBE5_SET() (GPIOB->BSRR |= GPIO_BSRR_BS5)
#define PROBE6_SET() (GPIOB->BSRR |= GPIO_BSRR_BS6)
#define PROBE7_SET() (GPIOB->BSRR |= GPIO_BSRR_BS7)
#define PROBE8_SET() (GPIOB->BSRR |= GPIO_BSRR_BS8)

#define PROBE1_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR1)
#define PROBE2_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR2)
#define PROBE3_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR3)
#define PROBE4_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR4)
#define PROBE5_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR5)
#define PROBE6_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR6)
#define PROBE7_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR7)
#define PROBE8_CLEAR() (GPIOB->BSRR |= GPIO_BSRR_BR8)

/* Macros for quick LED/BTN access */
#define LED_ON() (GPIOC->BSRR |= GPIO_BSRR_BR13)
#define LED_OFF() (GPIOC->BSRR |= GPIO_BSRR_BS13)

#define USR_BTN_READ() ((GPIOA->IDR & (1 << USR_BTN_PIN)) >> USR_BTN_PIN

/* LED */
#define LED_PIN (LL_GPIO_PIN_13)
#define LED_PORT (GPIOC)

/* BTN */
#define USR_BTN_PIN (LL_GPIO_PIN_0)
#define USR_BTN_PORT (GPIOA)

/* MIDI (UART1) */
#define MIDI_TX_PIN (LL_GPIO_PIN_9)  /* PA9 */
#define MIDI_RX_PIN (LL_GPIO_PIN_10) /* PA10 */
#define MIDI_AF (LL_GPIO_AF_7)       /* AF7 */
#define MIDI_PORT (GPIOA)

/* AUDIO (I2S2) */
#define I2S_WS_PIN (LL_GPIO_PIN_12)  /* B12 */
#define I2S_SDO_PIN (LL_GPIO_PIN_15) /* B15 */
#define I2S_SCK_PIN (LL_GPIO_PIN_10) /* B10 */
#define I2S_PORT (GPIOB)             /* GPIOB */
#define I2S_AF (LL_GPIO_AF_5)        /* AF5 */
#define I2S_MCK_PIN (LL_GPIO_PIN_3)  /* A3*/
#define I2S_MCK_PORT (GPIOA)         /* GPIOA */
