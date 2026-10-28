#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include <stdint.h>

void fsm_for_input_processing(void);

uint8_t get_current_mode(void);

uint8_t get_red_duration(void);

uint8_t get_red_editing(void);

uint8_t get_yellow_duration(void);

uint8_t get_yellow_editing(void);


#endif /* INC_INPUT_PROCESSING_H_ */
