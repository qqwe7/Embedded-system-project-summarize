/**
 ******************************************************************************
 * @file    pin_config.h
 * @brief   硬件引脚定义 (集中管理)
 ******************************************************************************
 */

#ifndef __PIN_CONFIG_H
#define __PIN_CONFIG_H

#include "stm32f10x.h"

/*-------------- HC-SR04 超声波模块引脚 --------------*/
#define HCSR04_TRIG_PORT        GPIOA
#define HCSR04_TRIG_PIN         GPIO_Pin_0       // PA2 - Trig
#define HCSR04_TRIG_RCC         RCC_APB2Periph_GPIOA

#define HCSR04_ECHO_PORT        GPIOA
#define HCSR04_ECHO_PIN         GPIO_Pin_1       // PA1 - Echo
#define HCSR04_ECHO_RCC         RCC_APB2Periph_GPIOA

/*-------------- 无源蜂鸣器引脚 --------------*/
#define BUZZER_PORT             GPIOB
#define BUZZER_PIN              GPIO_Pin_14      // PB12
#define BUZZER_RCC              RCC_APB2Periph_GPIOB

/*-------------- LED指示灯引脚 --------------*/
#define LED_PORT                GPIOB
#define LED_PIN                 GPIO_Pin_15      // PB15
#define LED_RCC                 RCC_APB2Periph_GPIOB

/*-------------- OLED 显示屏引脚 (I2C) --------------*/
#define OLED_SCL_PORT           GPIOB
#define OLED_SCL_PIN            GPIO_Pin_0       // PB8 - SCL
#define OLED_SCL_RCC            RCC_APB2Periph_GPIOB

#define OLED_SDA_PORT           GPIOB
#define OLED_SDA_PIN            GPIO_Pin_1       // PB9 - SDA
#define OLED_SDA_RCC            RCC_APB2Periph_GPIOB

/*-------------- USART1串口引脚 (WIFI模块) --------------*/
#define USART1_TX_PORT          GPIOA
#define USART1_TX_PIN           GPIO_Pin_9       // PA9
#define USART1_TX_RCC           RCC_APB2Periph_GPIOA

#define USART1_RX_PORT          GPIOA
#define USART1_RX_PIN           GPIO_Pin_10      // PA10
#define USART1_RX_RCC           RCC_APB2Periph_GPIOA

/*-------------- 按键引脚 --------------*/
#define KEY1_PORT               GPIOB
#define KEY1_PIN                GPIO_Pin_8       // PB0
#define KEY1_RCC                RCC_APB2Periph_GPIOB

#define KEY2_PORT               GPIOB
#define KEY2_PIN                GPIO_Pin_9       // PB1
#define KEY2_RCC                RCC_APB2Periph_GPIOB

#define KEY3_PORT               GPIOB
#define KEY3_PIN                GPIO_Pin_10       // PB3
#define KEY3_RCC                RCC_APB2Periph_GPIOB

#define KEY4_PORT               GPIOB
#define KEY4_PIN                GPIO_Pin_11       // PB4
#define KEY4_RCC                RCC_APB2Periph_GPIOB

#define KEY5_PORT               GPIOB
#define KEY5_PIN                GPIO_Pin_12       // PB5
#define KEY5_RCC                RCC_APB2Periph_GPIOB

#define KEY6_PORT               GPIOB
#define KEY6_PIN                GPIO_Pin_13       // PB6
#define KEY6_RCC                RCC_APB2Periph_GPIOB

/*-------------- 按键值定义 --------------*/
#define KEY_NONE                0
#define KEY1_PRESSED            1
#define KEY2_PRESSED            2
#define KEY3_PRESSED            3
#define KEY4_PRESSED            4
#define KEY5_PRESSED            5
#define KEY6_PRESSED            6

#endif /* __PIN_CONFIG_H */
