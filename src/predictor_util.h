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

uint32_t get_gshare_table_addr(uint32_t pc, uint32_t global_history, int ghistory_bits);
uint8_t get_two_bit_prediction_result(int twoBitsState);
uint8_t new_predictor_state(uint8_t old_state, uint8_t outcome);
uint32_t new_global_history_state(uint64_t old_global_history, uint8_t outcome);

#endif //SRC_PREDICTOR_UTIL_H
