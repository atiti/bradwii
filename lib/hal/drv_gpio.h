#pragma once

typedef enum
{
  Mode_IN   = 0x00,  /*!< GPIO Input Mode                */
  Mode_OUT  = 0x01,  /*!< GPIO Output Mode               */
  Mode_AF   = 0x02,  /*!< GPIO Alternate function Mode   */
  Mode_AN   = 0x03,  /*!< GPIO Analog In/Out Mode        */
  Mode_AF_PP = 0x00, /** Not supported, setting to input */
  Mode_IPD  = 0x00,  /** Not supported, setting to input */
  Mode_IPU  = 0x00,   /** Not supported, setting to input */
  Mode_IN_FLOATING  = 0x07,   /** Not supported, setting to input */
  Mode_Out_PP  = 0x08,   /** Not supported, setting to input */
  Mode_Out_OD = 0x09  /** Not supported setting to input */
} GPIO_Mode;

typedef enum
{
    Speed_10MHz = 0x01,
    Speed_2MHz = 0x02,
    Speed_50MHz = 0x03
} GPIO_Speed;

typedef enum
{
    Pin_0 = 0x0001,
    Pin_1 = 0x0002,
    Pin_2 = 0x0004,
    Pin_3 = 0x0008,
    Pin_4 = 0x0010,
    Pin_5 = 0x0020,
    Pin_6 = 0x0040,
    Pin_7 = 0x0080,
    Pin_8 = 0x0100,
    Pin_9 = 0x0200,
    Pin_10 = 0x0400,
    Pin_11 = 0x0800,
    Pin_12 = 0x1000,
    Pin_13 = 0x2000,
    Pin_14 = 0x4000,
    Pin_15 = 0x8000,
    Pin_All = 0xFFFF
} GPIO_Pin;

typedef struct
{
    uint16_t pin;
    GPIO_Mode mode;
    GPIO_Speed speed;
} gpio_config_t;

#define digitalHi(p, i)     { p->BSRR = i; }
#define digitalLo(p, i)     { p->BRR = i; }
#define digitalToggle(p, i) { p->ODR ^= i; }
#define digitalIn(p, i)     (p->IDR & i)

void gpioInit(GPIO_TypeDef *gpio, gpio_config_t *config);
