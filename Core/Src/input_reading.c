#include "main.h"
#include "input_reading.h"

#define NO_OF_BUTTONS 3U

// Button pins: PA1, PA2, PA3
static const uint16_t buttonPins[NO_OF_BUTTONS] =
{
    BUTTON_MODE_Pin,
    BUTTON_INC_Pin,
    BUTTON_SET_Pin
};

// Previous raw button states
static GPIO_PinState previousButtonState[NO_OF_BUTTONS] =
{
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    GPIO_PIN_SET
};

// Debounced button states
static volatile GPIO_PinState stableButtonState[NO_OF_BUTTONS] =
{
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    GPIO_PIN_SET
};

void button_reading(void)
{
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++)
    {
        GPIO_PinState currentButtonState =
            HAL_GPIO_ReadPin(GPIOA, buttonPins[i]);

        // Debouncing: compare two consecutive samples
        if (currentButtonState == previousButtonState[i])
        {
            stableButtonState[i] = currentButtonState;
        }

        // Save current reading
        previousButtonState[i] = currentButtonState;
    }
}

uint8_t is_button_pressed(uint8_t index)
{
    if (index >= NO_OF_BUTTONS)
    {
        return 0U;
    }

    return (stableButtonState[index] == GPIO_PIN_RESET);
}
