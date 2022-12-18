#include "utils.hpp"

float32_t clip(float32_t val, float32_t min, float32_t max){
    if (val < min){
        return min;
    }else if (val > max){
        return max;
    }else{
        return val;
    }
}