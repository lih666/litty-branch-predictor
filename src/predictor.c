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

int ghistoryBits; // Number of bits used for Global History
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
	int g_bht_size = ((1 << ghistoryBits) * sizeof(uint8_t) / 4) + 1;
	global_branch_history_table = malloc(g_bht_size * sizeof(uint8_t));
	memset(global_branch_history_table, DEFAULT_TWO_BITS_STATE, g_bht_size);
}

void init_tournament_predictor() {
	global_history = 0;
	int g_bht_size = ((1 << ghistoryBits) * sizeof(uint8_t) / 4) + 1;
	global_branch_history_table = malloc(g_bht_size * sizeof(uint8_t));
	memset(global_branch_history_table, DEFAULT_TWO_BITS_STATE, g_bht_size);


}

void init_custom_predictor() {

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

    return NOTTAKEN;
}

uint8_t make_custom_prediction(uint32_t pc) {

    return NOTTAKEN;
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
    global_history = new_global_history_state(global_history, outcome);
}

void train_tournament_predictor(uint32_t pc, uint8_t outcome) {

}

void train_custom_predictor(uint32_t pc, uint8_t outcome) {

}
