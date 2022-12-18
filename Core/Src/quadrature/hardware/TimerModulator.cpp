#include "TimerModulator.hpp"

#include "FOC_math.hpp"

void TimerModulator::initialize() {
    HAL_TIM_Base_Start(htimx);
}

void TimerModulator::hardwareEnable() {
    HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(htimx, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(htimx, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(htimx, TIM_CHANNEL_3);
}

void TimerModulator::hardwareDisable() {
    HAL_TIM_PWM_Stop(htimx, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(htimx, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(htimx, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(htimx, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(htimx, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(htimx, TIM_CHANNEL_3);
}

void TimerModulator::modulate(float32_t Va, float32_t Vb, float32_t Vc, float32_t Vdc) {
    VabcBuffer[0] = Va;
    VabcBuffer[1] = Vb;
    VabcBuffer[2] = Vc;

    forwardClarke(VabcBuffer, Vab0Buffer);

    modulate(Vab0Buffer, Vdc);
}

void TimerModulator::modulate(float32_t Vab0[3], float32_t Vdc) {
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