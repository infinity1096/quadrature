#ifndef AS5048_HPP
#define AS5048_HPP

#include "../Encoder.hpp"

class AS5048 : public Encoder{
    public:
    void initialize() override {

    }

    /*  
        for AS5048, define its raw information as the 14 bit data interpreted as unsigned.
    */
    void updateInternal(float32_t rawInformation, InformationValidity informationValid) override {
        
        if (informationValid == InformationValidity::INVALID){
            encoderState = ComponentState::ERROR;
            return;
        }

        int16_t absolute_angle = (int16_t)rawInformation - 8192;

        if (last_raw_information_initialized){
            last_absolute_angle = absolute_angle;
            last_raw_information_initialized = true;
            return;
        } 

        if (absolute_angle - last_absolute_angle > (ENCODER_PERIOD >> 1)){
            num_rotation--;
        }else if(absolute_angle - last_absolute_angle < -(ENCODER_PERIOD >> 1)){
            num_rotation++;
        }

        absolute_position = absolute_angle;
        last_absolute_angle = absolute_angle;
    }

    private:
    int16_t last_absolute_angle;
    bool last_raw_information_initialized = false;
};

#endif