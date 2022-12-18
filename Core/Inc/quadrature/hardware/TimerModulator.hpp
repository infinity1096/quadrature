#ifndef TIMER_MODULATOR_HPP
#define TIMER_MODULATOR_HPP

#include "../Modulator.hpp"
#include "../utils.hpp"
#include "tim.h"
#include "../FOC_math.hpp"

class TimerModulator : public Modulator {
    public:
    TimerModulator(TIM_HandleTypeDef* timer) : htimx(timer) {}

    void initialize() override {
        HAL_TIM_Base_Start(htimx);
    }

    void hardwareEnable() override {
        HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_2);
        HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_3);
        HAL_TIMEx_PWMN_Start(htimx, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Start(htimx, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Start(htimx, TIM_CHANNEL_3);
    }

    void hardwareDisable() override {
        HAL_TIM_PWM_Stop(htimx, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop(htimx, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop(htimx, TIM_CHANNEL_3);
        HAL_TIMEx_PWMN_Stop(htimx, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop(htimx, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop(htimx, TIM_CHANNEL_3);
    }

    void modulate(float32_t Va, float32_t Vb, float32_t Vc, float32_t Vdc) override {
        VabcBuffer[0] = Va;
        VabcBuffer[1] = Vb;
        VabcBuffer[2] = Vc;

        forwardClarke(VabcBuffer, Vab0Buffer);

        modulate(Vab0Buffer, Vdc);
    }

    void modulate(float32_t Vab0[3], float32_t Vdc) override {
        memcpy(Vab0Buffer, Vab0, 3 * sizeof(float32_t));

        float32_t voltageNorm = sqrt(Vab0Buffer[0] * Vab0Buffer[0] + Vab0Buffer[1] * Vab0Buffer[1]);
        
        if (voltageNorm > (2.0/3.0 * Vdc)){
            Vab0Buffer[0] = Vab0Buffer[0] / voltageNorm * (2.0/3.0 * Vdc);
            Vab0Buffer[1] = Vab0Buffer[1] / voltageNorm * (2.0/3.0 * Vdc);
        }

        SVPWM(Vab0Buffer, Vdc, timingBuffer);

        htimx->Instance->CCR1 = 1749 * timingBuffer[0];
        htimx->Instance->CCR2 = 1749 * timingBuffer[1];
        htimx->Instance->CCR3 = 1749 * timingBuffer[2];
    }

    private:
    TIM_HandleTypeDef* htimx;
    float32_t Vab0Buffer[3];
    float32_t VabcBuffer[3];
    float32_t timingBuffer[3];
};

#endif