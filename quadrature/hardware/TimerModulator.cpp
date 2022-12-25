#include "TimerModulator.hpp"

#include "FOC_math.hpp"
#include "safety.hpp"

#define USE_SVPWM

void TimerModulator::initialize() {
    HAL_TIM_Base_Start(htimx);
    safety_state = ComponentState::NORMAL;
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

    float32_t max_modulate = Vdc / 2.0;

    VabcBuffer[0] = clip(VabcBuffer[0], -max_modulate, max_modulate);
    VabcBuffer[1] = clip(VabcBuffer[1], -max_modulate, max_modulate);
    VabcBuffer[2] = clip(VabcBuffer[2], -max_modulate, max_modulate);
    
    htimx->Instance->CCR1 = 874.5 + 874 * VabcBuffer[0] / max_modulate;
    htimx->Instance->CCR2 = 874.5 + 874 * VabcBuffer[1] / max_modulate;
    htimx->Instance->CCR3 = 874.5 + 874 * VabcBuffer[2] / max_modulate;
    
}

void TimerModulator::modulate(float32_t Vab0[3], float32_t Vdc) {
    memcpy(Vab0Buffer, Vab0, 3 * sizeof(float32_t));

    float32_t voltageNorm = sqrt(Vab0Buffer[0] * Vab0Buffer[0] + Vab0Buffer[1] * Vab0Buffer[1]);
    
    if (voltageNorm > (2.0/3.0 * Vdc)){
        Vab0Buffer[0] = Vab0Buffer[0] / voltageNorm * (2.0/3.0 * Vdc);
        Vab0Buffer[1] = Vab0Buffer[1] / voltageNorm * (2.0/3.0 * Vdc);
    }

    SVPWM(Vab0Buffer, Vdc, timingBuffer);

    htimx->Instance->CCR1 = 3499 * timingBuffer[0];
    htimx->Instance->CCR2 = 3499 * timingBuffer[1];
    htimx->Instance->CCR3 = 3499 * timingBuffer[2];

    
}