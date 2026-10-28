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
// Current red light duration (seconds)
static uint8_t redDurationSec = 7U;

// Red light duration being edited
static uint8_t redEditingSec = 7U;

// Track button states for edge detection
static uint8_t incWasPressed = 0U;
static uint8_t setWasPressed = 0U;

void fsm_for_input_processing(void)
{
    // ===================================
    // 1. BUTTON MODE FSM
    // ===================================

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

                // Enter MODE 2
                if (currentMode == 2U)
                {
                    redEditingSec = redDurationSec;
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

    // ===================================
    // 2. READ INC AND SET BUTTONS
    // ===================================

    uint8_t incPressed =
        is_button_pressed(BUTTON_INC_INDEX);

    uint8_t setPressed =
        is_button_pressed(BUTTON_SET_INDEX);

    // ===================================
    // 3. MODIFY RED DURATION - MODE 2
    // ===================================

    if (currentMode == 2U)
    {
        // INC: Increase editing value
        if (incPressed && !incWasPressed)
        {
            redEditingSec++;

            // Valid range: 1 - 99 seconds
            if (redEditingSec > 99U)
            {
                redEditingSec = 1U;
            }
        }

        // SET: Save editing value
        if (setPressed && !setWasPressed)
        {
            redDurationSec = redEditingSec;
        }
    }

    // ===================================
    // 4. SAVE BUTTON STATES
    // ===================================

    incWasPressed = incPressed;
    setWasPressed = setPressed;
}

uint8_t get_current_mode(void)
{
    return currentMode;
}

uint8_t get_red_duration(void)
{
    return redDurationSec;
}

uint8_t get_red_editing(void)
{
    return redEditingSec;
}
