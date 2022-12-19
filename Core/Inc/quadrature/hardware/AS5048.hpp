#ifndef AS5048_HPP
#define AS5048_HPP

#include "../Encoder.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "spi.h"
#include "gpio.h"

#ifdef __cplusplus
}
#endif

class AS5048 : public Encoder{
    public:
    AS5048(SPI_HandleTypeDef* spix, GPIO_TypeDef* aGPIOx, uint16_t aGPIOPin) : hspix(spix), cs_GPIOx(aGPIOx), cs_GPIO_Pin(aGPIOPin) {}
    bool initialize() override;

    /*  
        for AS5048, define its raw information as the 14 bit data interpreted as unsigned.
    */
    void encoderReadCompleteCallback() override ;
    bool requestRead();

    private:
    int16_t last_absolute_angle;
    int last_information_initialized_cnt = 0;

    uint16_t spi_buffer = 0;
    uint16_t raw_receive = 0;

    SPI_HandleTypeDef* hspix;
    GPIO_TypeDef* cs_GPIOx; 
    uint16_t cs_GPIO_Pin;
};

#endif