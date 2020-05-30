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

uint32_t tournament_global_mask = 0;
uint32_t tournament_local_history_mask = 0;

uint64_t get_last_n_bits_64(uint64_t target, unsigned long n) {
	// Use for mask
	uint64_t mask = 0;
	for (int index = 0; index < n; index++) {
		mask = mask << 1;
		mask += 1;
	}

	return mask & target;
}

uint32_t get_last_n_bits_32(uint32_t v, int n) {
	v = v << (32 - n);
	v = v >> (32 - n);
	return v;
}


void get_gshare_table_addr(uint32_t pc, uint64_t global_history, unsigned long ghistory_bits, int* table_index, int* offset) {

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

void get_tournament_global_table_addr(uint32_t pc, uint64_t global_history, unsigned long ghistory_bits, int* table_index, int* offset) {
	if (tournament_global_mask == 0) {
		for (int index = 0; index < ghistory_bits; index++) {
			tournament_global_mask = tournament_global_mask << 1;
			tournament_global_mask += 1;
		}
	}

	uint64_t kept_global_history = global_history & tournament_global_mask;

	*table_index = (kept_global_history) / (GSHARE_ENTRY_SIZE / TWO_BITS);
	*offset = (kept_global_history) % (GSHARE_ENTRY_SIZE / TWO_BITS);
}

void get_tournament_local_prediction_addr(uint16_t local_history, unsigned long lhisotry_bits, int* table_index, int* offset) {
	if (tournament_local_history_mask == 0) {
		for (int index = 0; index < lhisotry_bits; index++) {
			tournament_local_history_mask = tournament_local_history_mask << 1;
			tournament_local_history_mask += 1;
		}
	}

	uint16_t kept_local_history = local_history & tournament_local_history_mask;

	*table_index = (kept_local_history) / (GSHARE_ENTRY_SIZE / TWO_BITS);
	*offset = (kept_local_history) % (GSHARE_ENTRY_SIZE / TWO_BITS);
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
	//global_history = new_history_state(global_history, outcome);
	uint8_t clean_mask = full_mask ^ (single_mask << (GSHARE_ENTRY_SIZE - (1 + offset) * TWO_BITS));
	uint8_t new_combined_state = old_combined_state & clean_mask;

	return new_combined_state | (new_state << (GSHARE_ENTRY_SIZE - (1 + offset) * TWO_BITS));
}

uint8_t new_selector_state(uint8_t old_state, uint8_t outcome, uint8_t global_prediction, uint8_t local_prediction) {

	if (outcome != global_prediction && outcome == local_prediction) {
		return new_predictor_state(old_state, USE_LOCAL);
	} else if (outcome == global_prediction && outcome != local_prediction) {
		return new_predictor_state(old_state, USE_GLOBAL);
	}

	return old_state;
}

uint32_t new_history_state(uint64_t old_history, uint8_t outcome) {
	return (old_history << 1) + outcome;
}

void custom_hashing(uint32_t pc, uint32_t* idx, uint32_t* tag, uint16_t* history) {
	uint32_t h[4];
	h[0] = history[0];
	h[1] = history[0] ^ history[1];
	h[2] = history[0] ^ history[1] ^ history[2] ^ history[3];
	h[3] = history[0] ^ history[1] ^ history[2] ^ history[3] ^ history[4] ^ history[5] ^ history[6] ^ history[7];

	for (int i = 0; i < 4; i++) {
		idx[i] = get_last_n_bits_32(pc ^ h[i], 10);
		tag[i] = get_last_n_bits_32((pc >> 10) ^ h[i], 8);
	}
}
