#include "main.h"
#include "input_reading.h"
#include "input_processing.h"

typedef enum
{
    BUTTON_RELEASED,
    BUTTON_PRESSED
} ButtonFSMState;

// Initial button state
static ButtonFSMState buttonState = BUTTON_RELEASED;

// Initial mode
static uint8_t currentMode = 1U;

void fsm_for_input_processing(void)
{
    switch (buttonState)
    {
        case BUTTON_RELEASED:

            if (is_button_pressed(BUTTON_MODE_INDEX))
            {
                currentMode++;

                if (currentMode > 4U)
                {
                    currentMode = 1U;
                }

                buttonState = BUTTON_PRESSED;
            }

            break;

        case BUTTON_PRESSED:

            if (!is_button_pressed(BUTTON_MODE_INDEX))
            {
                buttonState = BUTTON_RELEASED;
            }

            break;

        default:

            buttonState = BUTTON_RELEASED;
            break;
    }
}

uint8_t get_current_mode(void)
{
    return currentMode;
}
