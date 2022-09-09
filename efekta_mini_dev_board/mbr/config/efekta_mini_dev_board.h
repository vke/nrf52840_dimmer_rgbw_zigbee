#ifndef EFEKTA_DEV_BOARD_H
#define EFEKTA_DEV_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LED definitions for EFEKTA_DEV_BOARD_H
// Each LED color is considered a separate LED
#define LEDS_NUMBER    8

#define LED1_B         NRF_GPIO_PIN_MAP(0,6)
#define LED2_R         NRF_GPIO_PIN_MAP(0,8)
#define LED2_G         NRF_GPIO_PIN_MAP(1,9)
#define LED2_B         NRF_GPIO_PIN_MAP(0,12)

// rgbw dimmer daughter board
#define LED2_DW        NRF_GPIO_PIN_MAP(0,31)
#define LED2_DB        NRF_GPIO_PIN_MAP(0,30)
#define LED2_DG        NRF_GPIO_PIN_MAP(0,29)
#define LED2_DR        NRF_GPIO_PIN_MAP(0,26)

#define PSU_12V_SENSE_PIN NRF_GPIO_PIN_MAP(0,28)
#define PSU_ENABLE_PIN    NRF_GPIO_PIN_MAP(0,2)

#define LED_1          LED1_B
#define LED_2          LED2_R
#define LED_3          LED2_G
#define LED_4          LED2_B

#define LED_5          LED2_DR
#define LED_6          LED2_DG
#define LED_7          LED2_DB
#define LED_8          LED2_DW

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8 }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3
#define BSP_LED_3      LED_4

#define BSP_LED_4      LED_5
#define BSP_LED_5      LED_6
#define BSP_LED_6      LED_7
#define BSP_LED_7      LED_8

// There is only one button for the application
// as the second button is used for a RESET.
#define BUTTONS_NUMBER 1

#define BUTTON_1       NRF_GPIO_PIN_MAP(1, 6)
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1

#define BSP_SELF_PINRESET_PIN NRF_GPIO_PIN_MAP(0, 19)

#define HWFC           true

#ifdef __cplusplus
}
#endif

#endif // EFEKTA_DEV_BOARD_H
