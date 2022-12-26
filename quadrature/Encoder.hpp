#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "utils.hpp"
#include "safety.hpp"

#define ENCODER_PERIOD 16384L

class Encoder : public SafetyComponent{
    public:
    virtual bool initialize()=0;
    virtual void encoderReadCompleteCallback()=0;

    float32_t getAbsolutePosition();
    float32_t getAccumulatedPosition();

    bool is_data_valid = false;

    protected:
    int64_t absolute_position = 0;
    int64_t num_rotation = 0; 
};

#endif