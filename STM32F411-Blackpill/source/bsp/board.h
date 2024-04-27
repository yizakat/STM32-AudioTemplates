/**
 * @file board.h
 * @author yizakat (yizakat@yizakat.com)
 * @brief Board support configuration for Generic Blackpill boards (STM32F411CEU6)
 * @version 0.1
 * @date 2023-12-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef HARDWARE_BOARD_H_
#define HARDWARE_BOARD_H_

#include <stdint.h>
#include <system_stm32f4xx.h>
#include <stm32f411xe.h>
#include <stm32f4xx.h>

/* STM drivers */
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_utils.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_pwr.h>
#include <stm32f4xx_ll_dma.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_bus.h>

#include "pins.h"

#endif /* HARDWARE_BOARD_H_ */
