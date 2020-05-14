//
// Created by Lihao He on 5/10/20.
//
#include <stdio.h>
#include <stdlib.h>

#ifndef SRC_PREDICTOR_UTIL_H
#define SRC_PREDICTOR_UTIL_H

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

#ifndef USE_GLOBAL
#define USE_GLOBAL  0
#endif
#ifndef USE_LOCAL
#define USE_LOCAL   1
#endif

uint64_t get_last_n_bits(uint64_t target, unsigned long n);

void get_gshare_table_addr(uint32_t pc, uint64_t global_history, unsigned long ghistory_bits, int *table_index, int *offset);
void get_tournament_global_table_addr(uint32_t pc, uint64_t global_history, unsigned long ghistory_bits, int* table_index, int* offset);
void get_tournament_local_prediction_addr(uint16_t local_history, unsigned long lhisotry_bits, int* table_index, int* offset);

int get_two_bits_state(uint8_t full_prediction, int offset);
uint8_t get_two_bit_prediction_result(int twoBitsState);

uint8_t new_predictor_state(uint8_t old_state, uint8_t outcome);
uint8_t new_combined_state(uint8_t old_combined_state, uint8_t new_state, int offset);
uint8_t new_selector_state(uint8_t old_state, uint8_t outcome, uint8_t global_prediction, uint8_t local_prediction);

uint32_t new_history_state(uint64_t old_history, uint8_t outcome);

#endif //SRC_PREDICTOR_UTIL_H
