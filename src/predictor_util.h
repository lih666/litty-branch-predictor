//
// Created by Lihao He on 5/10/20.
//
#include <stdio.h>

#ifndef SRC_PREDICTOR_UTIL_H
#define SRC_PREDICTOR_UTIL_H

u_int32_t gshare_mask = 0;

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


u_int32_t get_gshare_table_addr(u_int32_t pc, int ghistoryBits) {

    // Use for XOR with pc
    if (gshare_mask == 0) {
        for (int index = 0; index < ghistoryBits; index++) {
            gshare_mask = gshare_mask << 1;
            gshare_mask += 1;
        }
    }

    return pc ^ gshare_mask;
}

u_int8_t get_two_bit_prediction_result(int twoBitsState) {

    if (twoBitsState <= WEAKLY_NOTTAKEN) {
        return NOTTAKEN;
    } else {
        return TAKEN;
    }
}

#endif //SRC_PREDICTOR_UTIL_H
