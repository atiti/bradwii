#include "hal.h"
#include "stm32f0xx_gpio.h"
#include "lib_serial.h"

#define DEBUGPORT SEMIHOSTPORTNUMBER

void gpioInit(GPIO_TypeDef *gpio, gpio_config_t *config)
{
    uint32_t pinpos;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);

    for (pinpos = 0; pinpos < 16; pinpos++) {
        if (config->pin & (0x1 << pinpos)) {
            // FIXME: We need to skip some pins otherwise the hardware would lock up...
	    if (DEBUGPORT &&
                ((gpio == GPIOA &&
                (pinpos == 0  ||
                 pinpos == 13 ||
                 pinpos == 14)) ||
                (gpio == GPIOB && pinpos > 7))) {
                continue;
	    }
            GPIO_InitStructure.GPIO_Pin = (0x1 << pinpos);
            GPIO_InitStructure.GPIO_Mode = config->mode;
            GPIO_InitStructure.GPIO_Speed = config->speed;

	    // Special handling for converting GPIO modes from stm32f1 to f0
	    switch (config->mode) {
                case Mode_IN_FLOATING:
                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
                    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
                    break;
                case Mode_Out_PP:
                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
                    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
                    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
                    break;
                case Mode_Out_OD:
                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
                    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 
                    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
                    break;
                default:
                    break;
            }

            GPIO_Init(GPIOA, &GPIO_InitStructure);     
        }
    }
}
