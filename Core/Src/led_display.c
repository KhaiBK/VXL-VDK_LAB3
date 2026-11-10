#include "main.h"
#include "led_display.h"

// Remaining time for each road
static uint8_t road1RemainingSec = 0U;
static uint8_t road2RemainingSec = 0U;

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

void sevenseg_test(void)
{
    // Disable all 7-segment displays
    HAL_GPIO_WritePin(
        GPIOB,
        EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin,
        GPIO_PIN_SET
    );

    // Turn off all segments
    HAL_GPIO_WritePin(
        GPIOB,
        SEGMENT_A_Pin | SEGMENT_B_Pin |
        SEGMENT_C_Pin | SEGMENT_D_Pin |
        SEGMENT_E_Pin | SEGMENT_F_Pin |
        SEGMENT_G_Pin,
        GPIO_PIN_SET
    );

    // Display number 1: turn on segments B and C
    HAL_GPIO_WritePin(
        GPIOB,
        SEGMENT_B_Pin | SEGMENT_C_Pin,
        GPIO_PIN_RESET
    );

    // Enable the first 7-segment display
    HAL_GPIO_WritePin(
        GPIOB,
        EN0_Pin,
        GPIO_PIN_RESET
    );
}

void sevenseg_scan_test(uint8_t mode, uint8_t value)
{
    static uint32_t lastScanMs = 0U;
    static uint8_t digitIndex = 0U;

    // Seven-segment patterns for digits 0-9
    static const uint8_t digitPatterns[10] =
    {
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x6F  // 9
    };

    static const uint16_t segmentPins[7] =
    {
        SEGMENT_A_Pin,
        SEGMENT_B_Pin,
        SEGMENT_C_Pin,
        SEGMENT_D_Pin,
        SEGMENT_E_Pin,
        SEGMENT_F_Pin,
        SEGMENT_G_Pin
    };

    static const uint16_t enablePins[4] =
    {
        EN0_Pin,
        EN1_Pin,
        EN2_Pin,
        EN3_Pin
    };

    // Two digits for MODE, two for value
    uint8_t digitValues[4] =
    {
        mode / 10U,
        mode % 10U,
        value / 10U,
        value % 10U
    };

    uint32_t now = HAL_GetTick();

    // Scan every 2 ms
    if ((uint32_t)(now - lastScanMs) < 2U)
    {
        return;
    }

    lastScanMs = now;

    // Disable all displays
    HAL_GPIO_WritePin(
        GPIOB,
        EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin,
        GPIO_PIN_SET
    );

    // Get pattern of current digit
    uint8_t pattern = digitPatterns[digitValues[digitIndex]];

    // Output segments
    for (uint8_t i = 0; i < 7U; i++)
    {
        if (pattern & (1U << i))
        {
            HAL_GPIO_WritePin(
                GPIOB,
                segmentPins[i],
                GPIO_PIN_RESET
            );
        }
        else
        {
            HAL_GPIO_WritePin(
                GPIOB,
                segmentPins[i],
                GPIO_PIN_SET
            );
        }
    }

    // Enable current display
    HAL_GPIO_WritePin(
        GPIOB,
        enablePins[digitIndex],
        GPIO_PIN_RESET
    );

    // Move to next display
    digitIndex++;

    if (digitIndex >= 4U)
    {
        digitIndex = 0U;
    }
}

static void update_traffic_countdown(
    TrafficState state,
    uint32_t elapsedMs,
    uint32_t green1Ms,
    uint32_t yellow1Ms,
    uint32_t green2Ms,
    uint32_t yellow2Ms)
{
    uint32_t road1Ms = 0U;
    uint32_t road2Ms = 0U;

    switch (state)
    {
        case ROAD1_GREEN_ROAD2_RED:

            road1Ms = green1Ms;
            road2Ms = green1Ms + yellow1Ms;
            break;

        case ROAD1_YELLOW_ROAD2_RED:

            road1Ms = yellow1Ms;
            road2Ms = yellow1Ms;
            break;

        case ROAD1_RED_ROAD2_GREEN:

            road1Ms = green2Ms + yellow2Ms;
            road2Ms = green2Ms;
            break;

        case ROAD1_RED_ROAD2_YELLOW:

            road1Ms = yellow2Ms;
            road2Ms = yellow2Ms;
            break;

        default:
            break;
    }

    // Subtract elapsed time
    road1Ms = (elapsedMs < road1Ms)
              ? road1Ms - elapsedMs : 0U;

    road2Ms = (elapsedMs < road2Ms)
              ? road2Ms - elapsedMs : 0U;

    // Convert milliseconds to seconds, rounding up
    road1RemainingSec =
        (uint8_t)((road1Ms + 999U) / 1000U);

    road2RemainingSec =
        (uint8_t)((road2Ms + 999U) / 1000U);
}

void traffic_light_update(uint8_t mode,
                          uint8_t blinkOn,
                          uint8_t redDurationSec,
                          uint8_t yellowDurationSec,
                          uint8_t greenDurationSec)
{
    static uint8_t previousMode = 0U;

    static TrafficState trafficState = ROAD1_GREEN_ROAD2_RED;

    static uint32_t stateStartMs = 0U;

    static uint16_t previousActivePins = 0xFFFFU;

    uint16_t activePins = 0U;

    uint32_t now = HAL_GetTick();

    // Detect MODE change
    if (mode != previousMode)
    {
        previousMode = mode;

        if (mode == 1U)
        {
            // Restart normal traffic light sequence
            trafficState = ROAD1_GREEN_ROAD2_RED;
            stateStartMs = now;
        }
    }

    // MODE 1: Normal traffic light
    if (mode == 1U)
    {
        uint32_t greenMs =
            (uint32_t)greenDurationSec * 1000U;

        uint32_t yellowMs =
            (uint32_t)yellowDurationSec * 1000U;

        uint32_t stateDuration = 0U;

        switch (trafficState)
        {
            case ROAD1_GREEN_ROAD2_RED:
                stateDuration = greenMs;
                break;

            case ROAD1_YELLOW_ROAD2_RED:
                stateDuration = yellowMs;
                break;

            case ROAD1_RED_ROAD2_GREEN:
                stateDuration = greenMs;
                break;

            case ROAD1_RED_ROAD2_YELLOW:
                stateDuration = yellowMs;
                break;

            default:
                trafficState = ROAD1_GREEN_ROAD2_RED;
                stateDuration = greenMs;
                stateStartMs = now;
                break;
        }

        // Move to next traffic state
        if ((uint32_t)(now - stateStartMs) >= stateDuration)
        {
            stateStartMs += stateDuration;

            if (trafficState == ROAD1_RED_ROAD2_YELLOW)
            {
                trafficState = ROAD1_GREEN_ROAD2_RED;
            }
            else
            {
                trafficState++;
            }
        }

        // Update countdown after changing state
        uint32_t stateElapsedMs = now - stateStartMs;

        update_traffic_countdown(
            trafficState,
            stateElapsedMs,
            greenMs,
            yellowMs,
            greenMs,
            yellowMs
        );

        // Determine active traffic LEDs
        switch (trafficState)
        {
            case ROAD1_GREEN_ROAD2_RED:
                activePins = LED_GREEN_Pin | LED_RED1_Pin;
                break;

            case ROAD1_YELLOW_ROAD2_RED:
                activePins = LED_YELLOW_Pin | LED_RED1_Pin;
                break;

            case ROAD1_RED_ROAD2_GREEN:
                activePins = LED_RED_Pin | LED_GREEN1_Pin;
                break;

            case ROAD1_RED_ROAD2_YELLOW:
                activePins = LED_RED_Pin | LED_YELLOW1_Pin;
                break;

            default:
                activePins = 0U;
                break;
        }
    }


    // MODE 2, 3, 4: Blink selected color
    else if (blinkOn == 1U)
    {
        switch (mode)
        {
            case 2:
                activePins = LED_RED_Pin | LED_RED1_Pin;
                break;

            case 3:
                activePins = LED_YELLOW_Pin | LED_YELLOW1_Pin;
                break;

            case 4:
                activePins = LED_GREEN_Pin | LED_GREEN1_Pin;
                break;

            default:
                break;
        }
    }

    // Update GPIO only when output state changes
    if (activePins != previousActivePins)
    {
        // Turn OFF all traffic LEDs
        HAL_GPIO_WritePin(
            GPIOA,
            LED_RED_Pin | LED_YELLOW_Pin | LED_GREEN_Pin |
            LED_RED1_Pin | LED_YELLOW1_Pin | LED_GREEN1_Pin,
            GPIO_PIN_RESET
        );

        // Turn ON selected LEDs
        if (activePins != 0U)
        {
            HAL_GPIO_WritePin(
                GPIOA,
                activePins,
                GPIO_PIN_SET
            );
        }

        previousActivePins = activePins;
    }
}

uint8_t get_road1_remaining(void)
{
    return road1RemainingSec;
}

uint8_t get_road2_remaining(void)
{
    return road2RemainingSec;
}
