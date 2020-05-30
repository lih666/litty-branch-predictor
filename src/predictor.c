//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

unsigned long ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

// Global History
// Used by: Gshare, Tournament
uint64_t global_history = 0;

// Global branch history table
// Used by: Gshare, Tournament
uint8_t* global_branch_history_table;

// Tournament prediction selector table, default to weakly select global predictor
uint8_t* tournament_selector_table;
uint16_t* tournament_local_history_table;       // From pc -> lhistoryBits history
uint8_t* tournament_local_history_prediction_table;     // From history bits -> 2 Bits prediction state

uint8_t* bimodal_ctr;
uint8_t* bimodal_m;

uint8_t* ctr[4];
uint8_t* tag[4];
uint8_t* u[4];
uint64_t ghistory;
uint16_t history[8];


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_predictor() {
	// Make a prediction based on the bpType
	switch (bpType) {
		case STATIC:
			return;
		case GSHARE:
			return init_gshare_predictor();
		case TOURNAMENT:
			return init_tournament_predictor();
		case CUSTOM:
			return init_custom_predictor();
		default:
			break;
	}
}

void init_gshare_predictor() {
	global_history = 0;
	unsigned long one = 1;
	unsigned long g_bht_size = (((one << ghistoryBits) * sizeof(uint8_t)) / (GSHARE_ENTRY_SIZE / TWO_BITS)) + 1;

	/*
    printf("Size of size_t: %lu \n", sizeof(size_t));
    printf("Size of uint8_t: %lu \n", sizeof(uint8_t));
    printf("Shift bits: %lu \n", ghistoryBits);
    printf("Original number of bytes: %lu \n", ((one << ghistoryBits) * sizeof(uint8_t)));
    printf("Number of bytes to allocate: %lu \n", g_bht_size);
    */

	global_branch_history_table = malloc(g_bht_size * sizeof(uint8_t));
	memset(global_branch_history_table, DEFAULT_TWO_BITS_STATE, g_bht_size);
}

void init_tournament_predictor() {
	global_history = 0;

	unsigned long one = 1;

	/* global history related */
	unsigned long g_bht_size = (((one << ghistoryBits) * sizeof(uint8_t)) / 4) + 1;

	global_branch_history_table = malloc(g_bht_size * sizeof(uint8_t));
	memset(global_branch_history_table, DEFAULT_TWO_BITS_STATE, g_bht_size);

	tournament_selector_table = malloc(g_bht_size * sizeof(uint8_t));
	memset(tournament_selector_table, DEFAULT_TWO_BITS_STATE, g_bht_size);

	/* local history predictor related */
	unsigned long l_history_table_size = ((one << pcIndexBits) * sizeof(uint16_t)) + 1;
	tournament_local_history_table = malloc(l_history_table_size);
	memset(tournament_local_history_table, 0, l_history_table_size);

	unsigned long l_history_prediction_size = (((one << lhistoryBits) * sizeof(uint8_t)) / (TOURNAMENT_L_PREDICTION_ENTRY_SIZE/TWO_BITS)) + 1;
	tournament_local_history_prediction_table = malloc(l_history_prediction_size);
	memset(tournament_local_history_prediction_table, DEFAULT_TWO_BITS_STATE, l_history_prediction_size);
}

void init_custom_predictor() {
	uint8_t* tmp = (uint8_t*)malloc(4096 * 5 * sizeof(uint8_t));
	memset(tmp, 0, 4096 * 5 * sizeof(uint8_t));
	memset(tmp, 3, 4096 * 2 * sizeof(uint8_t));
	bimodal_ctr = tmp;
	tmp += 4096;
	ctr[0] = tmp;
	tmp += 1024;
	ctr[1] = tmp;
	tmp += 1024;
	ctr[2] = tmp;
	tmp += 1024;
	ctr[3] = tmp;
	tmp += 1024;
	bimodal_m = tmp;
	tmp += 4096;
	tag[0] = tmp;
	tmp += 1024;
	tag[1] = tmp;
	tmp += 1024;
	tag[2] = tmp;
	tmp += 1024;
	tag[3] = tmp;
	tmp += 1024;
	u[0] = tmp;
	tmp += 1024;
	u[1] = tmp;
	tmp += 1024;
	u[2] = tmp;
	tmp += 1024;
	u[3] = tmp;
	tmp += 1024;
	memset(history, 0, 8 * sizeof(uint16_t));
	ghistory = 0;
}


// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc) {

	// Make a prediction based on the bpType
	switch (bpType) {
		case STATIC:
			return TAKEN;
		case GSHARE:
			return make_gshare_prediction(pc);
		case TOURNAMENT:
			return make_tournament_prediction(pc);
		case CUSTOM:
			return make_custom_prediction(pc);
		default:
			break;
	}

	// If there is not a compatible bpType then return NOTTAKEN
	return NOTTAKEN;
}


uint8_t make_gshare_prediction(uint32_t pc) {
	int table_index = 0;
	int entry_offset = 0;

	// Determine the entry in the prediction table
	get_gshare_table_addr(pc, global_history, ghistoryBits, &table_index, &entry_offset);

	// Fetch the corresponding prediction result in the table
	uint8_t combined_states = global_branch_history_table[table_index];
	int single_state = get_two_bits_state(combined_states, entry_offset);

	return get_two_bit_prediction_result(single_state);
}

uint8_t make_tournament_prediction(uint32_t pc) {

	// Global Prediction
	int g_table_index, g_entry_offset;
	get_tournament_global_table_addr(pc, global_history, ghistoryBits, &g_table_index, &g_entry_offset);
	uint8_t g_combined_states = global_branch_history_table[g_table_index];
	int global_state = get_two_bits_state(g_combined_states, g_entry_offset);

	// Local Prediction
	int l_table_index, l_entry_offset;
	uint32_t kept_pc_index = get_last_n_bits_32(pc, pcIndexBits);
	uint16_t local_history = tournament_local_history_table[kept_pc_index];
	get_tournament_local_prediction_addr(local_history, lhistoryBits, &l_table_index, &l_entry_offset);
	uint8_t l_combined_states = tournament_local_history_prediction_table[l_table_index];
	int local_state = get_two_bits_state(l_combined_states, l_entry_offset);

	// Selector decision
	uint8_t selector_combined_states = tournament_selector_table[g_table_index];
	int selector_state = get_two_bits_state(selector_combined_states, g_entry_offset);
	int selector_result = get_two_bit_prediction_result(selector_state);
//
//	printf("GLOBAL HISTORY:%x", global_history);
//	printf("GLOBAL %d:%d\n", global_state, global_prediction);
//	printf("LOCAL %d:%d\n", local_state, local_prediction);
//	printf("USE %d:%d\n", selector_state, selector_result);

	if (selector_result == USE_GLOBAL) {
		return get_two_bit_prediction_result(global_state);
	} else {
		return get_two_bit_prediction_result(local_state);
	}
}

uint8_t make_custom_prediction(uint32_t pc) {

	uint32_t idx0 = get_last_n_bits_32(pc, 12);
	uint32_t idx[4];
	uint32_t t_tag[4];
	custom_hashing(pc, idx, t_tag, history);
	for (int i = 3; i > -1; i--) {

		if (t_tag[i] == tag[i][idx[i]]) return ctr[i][idx[i]] < 4 ? NOTTAKEN : TAKEN;

	}

	return bimodal_ctr[idx0] < 4 ? NOTTAKEN : TAKEN;
}



// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome) {
	switch (bpType) {
		case STATIC:
			return;
		case GSHARE:
			return train_gshare_predictor(pc, outcome);
		case TOURNAMENT:
			return train_tournament_predictor(pc, outcome);
		case CUSTOM:
			return train_custom_predictor(pc, outcome);
		default:
			break;
	}
}


void train_gshare_predictor(uint32_t pc, uint8_t outcome) {
	int table_index = 0;
	int entry_offset = 0;

	// Determine the entry in the prediction table
	get_gshare_table_addr(pc, global_history, ghistoryBits, &table_index, &entry_offset);

	// Fetch the corresponding prediction result in the table
	uint8_t combined_states = global_branch_history_table[table_index];
	uint8_t old_state = get_two_bits_state(combined_states, entry_offset);

	// Update global branch history state for depending on current state
	uint8_t new_state = new_predictor_state(old_state, outcome);
	uint8_t new_combined_states = new_combined_state(combined_states, new_state, entry_offset);
	global_branch_history_table[table_index] = new_combined_states;
	global_history = new_history_state(global_history, outcome);
}

void train_tournament_predictor(uint32_t pc, uint8_t outcome) {

	// Global Prediction
	int g_table_index, g_entry_offset;
	get_tournament_global_table_addr(pc, global_history, ghistoryBits, &g_table_index, &g_entry_offset);
	uint8_t g_combined_states = global_branch_history_table[g_table_index];
	int global_state = get_two_bits_state(g_combined_states, g_entry_offset);
	int global_prediction = get_two_bit_prediction_result(global_state);

	// Local Prediction
	int l_table_index, l_entry_offset;
	uint32_t kept_pc_index = get_last_n_bits_32(pc, pcIndexBits);
	uint16_t local_history = tournament_local_history_table[kept_pc_index];
	get_tournament_local_prediction_addr(local_history, lhistoryBits, &l_table_index, &l_entry_offset);
	uint8_t l_combined_states = tournament_local_history_prediction_table[l_table_index];
	int local_state = get_two_bits_state(l_combined_states, l_entry_offset);
	int local_prediction = get_two_bit_prediction_result(local_state);

	// Selector decision
	uint8_t selector_combined_states = tournament_selector_table[g_table_index];
	int selector_state = get_two_bits_state(selector_combined_states, g_entry_offset);

	// Update selector
	uint8_t updated_selector_state = new_selector_state(selector_state, outcome, global_prediction, local_prediction);
	uint8_t updated_combined_selector_states = new_combined_state(selector_combined_states, updated_selector_state, g_entry_offset);
	tournament_selector_table[g_table_index] = updated_combined_selector_states;

	// Update predictor states
	uint8_t g_new_predictor_states = new_predictor_state(global_state, outcome);
	uint8_t g_new_combined_predictor_states = new_combined_state(g_combined_states, g_new_predictor_states, g_entry_offset);
	global_branch_history_table[g_table_index] = g_new_combined_predictor_states;

	uint8_t l_new_predictor_states = new_predictor_state(local_state, outcome);
	uint8_t l_new_combined_predictor_states = new_combined_state(l_combined_states, l_new_predictor_states, l_entry_offset);
	tournament_local_history_prediction_table[l_table_index] = l_new_combined_predictor_states;

	// Update history
	global_history = new_history_state(global_history, outcome);
	uint16_t new_local_history = new_history_state(local_history, outcome);
	tournament_local_history_table[kept_pc_index] = new_local_history;
//
//	printf("OUTCOME IS %d\n", outcome);
//	printf("T-GLOBAL %d\n", g_new_predictor_states);
//	printf("T-LOCAL %d\n", l_new_predictor_states);
//	printf("T-USE %d\n", updated_selector_state);
//	printf("--------------\n");

}

void train_custom_predictor(uint32_t pc, uint8_t outcome) {
	ghistory++;

	uint32_t idx0 = get_last_n_bits_32(pc, 12);
	uint32_t idx[4];
	uint32_t t_tag[4];
	uint8_t res = bimodal_ctr[idx0] < 4 ? NOTTAKEN : TAKEN;
	custom_hashing(pc, idx, t_tag, history);

	int i = 3;
	for (i = 3; i > -1; i--)
		if (t_tag[i] == tag[i][idx[i]]) {
			res = ctr[i][idx[i]] < 4 ? NOTTAKEN : TAKEN;
			break;
		}
	if (res != (bimodal_ctr[idx0] < 4 ? NOTTAKEN : TAKEN)) {
		if (res == outcome) {
			bimodal_m[idx0] = 1;
			u[i][idx[i]] = 1;
		}
		else {
			bimodal_m[idx0] = 1;
			u[i][idx[i]] = 1;
		}
	}
	if (i == -1) {
		if (outcome == NOTTAKEN) bimodal_ctr[idx0] = bimodal_ctr[idx0] == 0 ? 0 : bimodal_ctr[idx0] - 1;
		else bimodal_ctr[idx0] = bimodal_ctr[idx0] == 7 ? 7 : bimodal_ctr[idx0] + 1;
	}
	else {
		if (outcome == NOTTAKEN) ctr[i][idx[i]] = ctr[i][idx[i]] == 0 ? 0 : ctr[i][idx[i]] - 1;
		else ctr[i][idx[i]] = ctr[i][idx[i]] == 7 ? 7 : ctr[i][idx[i]] + 1;
	}

	if (i != 3 && res != outcome) {
		int j = i + 1;
		for (; j < 4; j++) if (u[j][idx[j]] == 0) break;

		if (j == 4) j = pc % (4 - i - 1) + i + 1;

		u[j][idx[j]] = 0;

		tag[j][idx[j]] = t_tag[j];
		if (bimodal_m[idx0] == 1) ctr[j][idx[j]] = outcome == NOTTAKEN ? 3 : 4;
		else ctr[j][idx[j]] = bimodal_ctr[idx0] < 4 ? 3 : 4;
	}


	uint32_t carry = outcome == NOTTAKEN ? 0 : 1;
	for (int i = 0; i < 8; i++) {
		history[i] = (history[i] << 1) + carry;
		carry = (history[i] & 1024*32) == 0 ? 0 : 1;
	}

}
