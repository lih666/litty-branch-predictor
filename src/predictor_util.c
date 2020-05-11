//
// Created by Lihao He on 5/10/20.
//
#include <stdio.h>
#include "predictor_util.h"
uint32_t gshare_mask = 0;

uint32_t get_gshare_table_addr(uint32_t pc, uint32_t global_history, int ghistory_bits) {

	// Use for XOR with pc
	if (gshare_mask == 0) {
		for (int index = 0; index < ghistory_bits; index++) {
			gshare_mask = gshare_mask << 1;
			gshare_mask += 1;
		}
	}

	// Only keep the last "ghisotry_bits" by ANDing with mask
	return (pc ^ global_history) & gshare_mask;
}

uint8_t get_two_bit_prediction_result(int twoBitsState) {
	if (twoBitsState <= WEAKLY_NOTTAKEN) {
		return NOTTAKEN;
	} else {
		return TAKEN;
	}
}

uint8_t new_predictor_state(uint8_t old_state, uint8_t outcome) {
	uint8_t new_state;

	if (outcome == TAKEN) {
		new_state = MIN(old_state + 1, STRONGLY_TAKEN);
	} else {
		new_state = MAX(old_state - 1, STRONGLY_NOTTAKEN);
	}

	return new_state;
}

uint32_t new_global_history_state(uint64_t old_global_history, uint8_t outcome) {
	return (old_global_history << 1) + outcome;
}