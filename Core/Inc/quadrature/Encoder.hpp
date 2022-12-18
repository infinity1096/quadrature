#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "utils.hpp"
#include "safety.hpp"

#define ENCODER_PERIOD 16384L

class Encoder{
    public:
    virtual void initialize()=0;
    virtual void updateInternal(float32_t rawInformation, InformationValidity informationValid)=0;

    float32_t getAbsolutePosition(){
        return (float32_t) absolute_position / (float32_t) ENCODER_PERIOD * M_TWOPI;
    }

    float32_t getAccumulatedPosition(){
        return getAbsolutePosition() + M_TWOPI * num_rotation;
    }

    ComponentState getState(){
        return encoderState;
    }

    protected:
    int64_t absolute_position;
    int64_t num_rotation; 

    ComponentState encoderState;
};

#endif