//
// Created by Lihao He on 5/10/20.
//
#include <stdio.h>

#ifndef SRC_PREDICTOR_UTIL_H
#define SRC_PREDICTOR_UTIL_H

u_int32_t gshare_mask = 0;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#ifndef TAKEN
#define TAKEN       1
#endif
#ifndef NOTTAKEN
#define NOTTAKEN    0
#endif
#ifndef STRONGLY_NOTTAKEN
#define STRONGLY_NOTTAKEN   0
#endif
#ifndef WEAKLY_NOTTAKEN
#define WEAKLY_NOTTAKEN 1
#endif
#ifndef WEAKLY_TAKEN
#define WEAKLY_TAKEN    2
#endif
#ifndef STRONGLY_TAKEN
#define STRONGLY_TAKEN  3
#endif


u_int32_t get_gshare_table_addr(u_int32_t pc, u_int32_t global_history, int ghistory_bits) {

    // Use for XOR with pc
    if (gshare_mask == 0) {
        for (int index = 0; index < ghistory_bits; index++) {
            gshare_mask = gshare_mask << 1;
            gshare_mask += 1;
        }
    }

    // Only keep the last "ghisotry_bits" by AND with mask
    return (pc ^ global_history) & gshare_mask;
}

u_int8_t get_two_bit_prediction_result(int twoBitsState) {
    if (twoBitsState <= WEAKLY_NOTTAKEN) {
        return NOTTAKEN;
    } else {
        return TAKEN;
    }
}

u_int8_t new_predictor_state(uint8_t old_state, uint8_t outcome) {
	u_int8_t new_state;

	if (outcome == TAKEN) {
		new_state = MIN(old_state + 1, STRONGLY_TAKEN);
	} else {
		new_state = MAX(old_state - 1, STRONGLY_NOTTAKEN);
	}

	return new_state;
}

uint32_t new_global_history_state(uint32_t old_global_history, uint8_t outcome) {
	return (old_global_history << 1) + outcome;
}

#endif //SRC_PREDICTOR_UTIL_H
