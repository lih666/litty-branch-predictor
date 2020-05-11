//
// Created by Lihao He on 5/10/20.
//
#include <stdio.h>
#include "predictor_util.h"


#ifndef TWO_BITS
#define TWO_BITS 2
#endif

#ifndef GSHARE_ENTRY_SIZE
#define GSHARE_ENTRY_SIZE 8
#endif

uint32_t gshare_mask = 0;
uint8_t single_mask = 0b11;
uint8_t full_mask = 0xFF;

void get_gshare_table_addr(uint32_t pc, uint64_t global_history, int ghistory_bits, int* table_index, int* offset) {

	// Use for XOR with pc
	if (gshare_mask == 0) {
		for (int index = 0; index < ghistory_bits; index++) {
			gshare_mask = gshare_mask << 1;
			gshare_mask += 1;
		}
	}

	// Only keep the last "ghisotry_bits" by ANDing with mask
	*table_index = ((pc ^ global_history) & gshare_mask) / (GSHARE_ENTRY_SIZE / TWO_BITS);
	*offset = ((pc ^ global_history) & gshare_mask) % (GSHARE_ENTRY_SIZE / TWO_BITS);
}

int get_two_bits_state(uint8_t full_prediction, int offset) {
	return full_prediction >> ((GSHARE_ENTRY_SIZE - ((1 + offset) * TWO_BITS))) & single_mask;
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

uint8_t new_combined_state(uint8_t old_combined_state, uint8_t new_state, int offset) {
	//global_history = new_global_history_state(global_history, outcome);
	uint8_t clean_mask = full_mask ^ (single_mask << (GSHARE_ENTRY_SIZE - (1 + offset) * TWO_BITS));
	uint8_t new_combined_state = old_combined_state & clean_mask;

	return new_combined_state | (new_state << (GSHARE_ENTRY_SIZE - (1 + offset) * TWO_BITS));
}


uint32_t new_global_history_state(uint64_t old_global_history, uint8_t outcome) {
	return (old_global_history << 1) + outcome;
}