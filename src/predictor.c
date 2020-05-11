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

}

void init_tournament_predictor() {

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

    return NOTTAKEN;
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

}

void train_tournament_predictor(uint32_t pc, uint8_t outcome) {

}

void train_custom_predictor(uint32_t pc, uint8_t outcome) {

}
