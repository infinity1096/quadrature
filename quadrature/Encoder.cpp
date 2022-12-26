#include "Encoder.hpp"



float32_t Encoder::getAbsolutePosition(){
    return absolute_position / ENCODER_PERIOD * M_TWOPI;
}

float32_t Encoder::getAccumulatedPosition(){
    return getAbsolutePosition() + M_TWOPI * num_rotation;
}