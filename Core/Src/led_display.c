#include "main.h"
#include "led_display.h"

void led_display_mode(uint8_t mode)
{
    // LED GREEN - PA7
    if (mode == 2U || mode == 4U)
    {
        HAL_GPIO_WritePin(
            GPIOA,
            LED_GREEN_Pin,
            GPIO_PIN_SET
        );
    }
    else
    {
        HAL_GPIO_WritePin(
            GPIOA,
            LED_GREEN_Pin,
            GPIO_PIN_RESET
        );
    }

    // LED RED1 - PA8
    if (mode == 3U || mode == 4U)
    {
        HAL_GPIO_WritePin(
            GPIOA,
            LED_RED1_Pin,
            GPIO_PIN_SET
        );
    }
    else
    {
        HAL_GPIO_WritePin(
            GPIOA,
            LED_RED1_Pin,
            GPIO_PIN_RESET
        );
    }
}
