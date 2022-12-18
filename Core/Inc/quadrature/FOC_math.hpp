#ifndef __FOC_MATH_HPP
#define __FOC_MATH_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"
#include "math.h"

#ifdef __cplusplus
}
#endif

#define USE_FPU_FOR_FOC_MATH

#define SQRT3      1.73205080756888f
#define INV_SQRT3  0.577350269189626f
#define SQRT3_DIV2 0.866025403784439f
#define SQRT3_DIV3 0.577350269189626f


void forwardClarke(float32_t Vabc[3], float32_t Vab0[3]);
void inverseClarke(float32_t Vab0[3], float32_t Vabc[3]);
void forwardPark(float32_t Vab0[3], float32_t theta_e, float32_t Vdq0[3]);
void inversePark(float32_t Vdq0[3], float32_t theta_e, float32_t Vab0[3]);

void SVPWM(float32_t Vab0Input[3], float32_t Vdc, float32_t tABC[3]);

#endif