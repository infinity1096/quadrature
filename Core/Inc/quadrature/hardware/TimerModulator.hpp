#ifndef __TIMER_MODULATOR_HPP
#define __TIMER_MODULATOR_HPP

#include "../Modulator.hpp"
#include "../utils.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "tim.h"

#ifdef __cplusplus
}
#endif

class TimerModulator : public Modulator {
    public:
    TimerModulator(TIM_HandleTypeDef* timer) : htimx(timer) {}

    void initialize() override ;

    void hardwareEnable() override ;
    void hardwareDisable() override ;

    void modulate(float32_t Va, float32_t Vb, float32_t Vc, float32_t Vdc) override;

    void modulate(float32_t Vab0[3], float32_t Vdc) override ;

    private:
    TIM_HandleTypeDef* htimx;
    float32_t Vab0Buffer[3];
    float32_t VabcBuffer[3];
    float32_t timingBuffer[3];
};

#endif