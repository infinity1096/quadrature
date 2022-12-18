#ifndef __ENCODER_HPP
#define __ENCODER_HPP

// C includes
#ifdef __cplusplus
extern "C" {
#endif

// add C includes here
#include "stm32f4xx_hal.h"
#include "arm_math.h"

#ifdef __cplusplus
}
#endif

// C++ includes
#include "stm32gpio.hpp"

#define ABSOLUTE_ANGLE_NOT_INITIALIZED -10.0f

class IEncoder {
    public:

    //initialize
    virtual bool initialize()=0;

    // initiate read from encoder
    virtual bool initiateEncoderRead()=0;
    virtual bool encoderReadCompleteCallback()=0;

    // read result
    float32_t getAbsoluteAngle(){
        this->is_valid = false;
        return this->absolute_angle;
    }

    float32_t getAccumulatedAngle(){
        this->is_valid = false;
        return this->accumulated_angle;
    }

    bool is_data_valid(){
        return is_valid;
    }

    protected:
    // the above differs in terms of whether angle warp-around is accumulated. 
    // if a motor rotates in positive direction for 2.5pi rad, we have:
    // absolute_angle       = (2.5pi + pi) % (2pi) - pi = 0.5pi
    // accumulated_angle    = 2.5pi

    float32_t absolute_angle = 0;           // value in [-pi,pi)
    float32_t accumulated_angle = 0;        // total distance traveled in angles.  
    
    bool is_valid = false;                  // set to true when new data is collected.
                                            // set to false when read.
};

class AS5048 : public IEncoder {
    public:
    //constructor
    AS5048(SPI_HandleTypeDef* spix, STM32Gpio CS_pin) : spix(spix), CS_pin(CS_pin) {};
    AS5048(SPI_HandleTypeDef* spix, GPIO_TypeDef* port, uint16_t pin) : spix(spix), CS_pin(port, pin) {};

    //initialize
    bool initialize();

    // initiate read from encoder
    bool initiateEncoderRead();
    bool encoderReadCompleteCallback();

    int32_t getRawReceive();

    private:
    SPI_HandleTypeDef* spix;
    STM32Gpio CS_pin;

    uint16_t raw_receive;
    uint16_t raw_buffer;

    float32_t last_absolute_angle = ABSOLUTE_ANGLE_NOT_INITIALIZED; 

    int32_t accumulated_angle_turns = 0;
};

#endif