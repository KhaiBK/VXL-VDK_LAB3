#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include <stdint.h>


#define TRAFFIC_GREEN_MS   5000U
#define TRAFFIC_YELLOW_MS  2000U

typedef enum
{
    ROAD1_GREEN_ROAD2_RED,
    ROAD1_YELLOW_ROAD2_RED,
    ROAD1_RED_ROAD2_GREEN,
    ROAD1_RED_ROAD2_YELLOW
} TrafficState;


void led_display_mode(uint8_t mode);
void sevenseg_test(void);
void sevenseg_scan_test(uint8_t mode);
void traffic_light_update(uint8_t mode, uint8_t blinkOn);

#endif /* INC_LED_DISPLAY_H_ */
