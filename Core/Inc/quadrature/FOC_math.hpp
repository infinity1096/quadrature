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


__INLINE void forwardClarke(float32_t Vabc[3], float32_t Vab0[3]){
    Vab0[0] = 2.0f /3.0f * Vabc[0] - Vabc[1]/3.0f - Vabc[2]/3.0f;
    Vab0[1] = SQRT3_DIV3 * Vabc[1] - SQRT3_DIV3 * Vabc[2];
    Vab0[2] = (Vabc[0] + Vabc[1] + Vabc[2]) / 3.0f;
}

__INLINE void inverseClarke(float32_t Vab0[3], float32_t Vabc[3]){
    Vabc[0] = Vab0[0] + Vab0[2];
    Vabc[1] = -0.5f * Vab0[0] + SQRT3_DIV2 * Vab0[1] + Vab0[2];
    Vabc[2] = -0.5f * Vab0[0] - SQRT3_DIV2 * Vab0[1] + Vab0[2];
}

__INLINE void forwardPark(float32_t Vab0[3], float32_t theta_e, float32_t Vdq0[3]){
    float32_t sin_theta, cos_theta;
    #ifdef USE_FPU_FOR_FOC_MATH
        sin_theta = arm_sin_f32(theta_e);
        cos_theta = arm_cos_f32(theta_e);
    #else
        sin_theta = sin(theta_e);
        cos_theta = cos(theta_e);
    #endif

    Vdq0[0] =  cos_theta * Vab0[0] + sin_theta * Vab0[1];
    Vdq0[1] = -sin_theta * Vab0[0] + cos_theta * Vab0[1];
    Vdq0[2] = Vab0[2];
}

__INLINE void inversePark(float32_t Vdq0[3], float32_t theta_e, float32_t Vab0[3]){
    float32_t sin_theta, cos_theta;
    #ifdef USE_FPU_FOR_FOC_MATH        
        sin_theta = arm_sin_f32(theta_e);
        cos_theta = arm_cos_f32(theta_e);
    #else
        sin_theta = sin(theta_e);
        cos_theta = cos(theta_e);
    #endif

    Vab0[0] = cos_theta * Vdq0[0] - sin_theta * Vdq0[1];
    Vab0[1] = sin_theta * Vdq0[0] + cos_theta * Vdq0[1];
    Vab0[2] = Vdq0[2];
}


__INLINE void SVPWM(float32_t Vab0Input[3], float32_t Vdc, float32_t tABC[3]){
    //this section compute tA, tB, and tC from alpha and beta.
    int sector = 0;
    float t_vec1, t_vec2;
    float32_t Vab0[3];

    Vab0[0] = Vab0Input[0] / (2.0/3.0 * Vdc);
    Vab0[1] = Vab0Input[1] / (2.0/3.0 * Vdc);
    //Vab0[2] = Vab0Input[2] / (2.0/3.0 * Vdc);
    
    if(Vab0[1] > 0){
        if(Vab0[1] < Vab0[0] * SQRT3){
                //sector 1
                t_vec1 = Vab0[0] - INV_SQRT3 * Vab0[1]; //Time for U4
                t_vec2 = 2 * INV_SQRT3 * Vab0[1];     //Time for U6
                sector = 1;
        }else{
            if(Vab0[1] > -Vab0[0] * SQRT3){
                //sector 2
                t_vec1 = Vab0[0] + INV_SQRT3 * Vab0[1]; //Time for U6
                t_vec2 = -Vab0[0] + INV_SQRT3 * Vab0[1];//Time for U2
                sector = 2;
            }else{
                //sector 3
                t_vec1 = 2 * INV_SQRT3 * Vab0[1];     //Time for U2
                t_vec2 = -Vab0[0] - INV_SQRT3 * Vab0[1];//Time for U3
                sector = 3;            
            }
        }
    }else{
        if(-Vab0[1] < Vab0[0] * SQRT3){
                //sector 6
                t_vec1 = Vab0[0] + INV_SQRT3 * Vab0[1]; //Time for U4
                t_vec2 = - 2 * INV_SQRT3 * Vab0[1];   //Time for U5
                sector = 6;     
        }else{
            if(-Vab0[1] > -Vab0[0] * SQRT3){
                //sector 5
                t_vec1 = Vab0[0] - INV_SQRT3 * Vab0[1]; //Time for U5
                t_vec2 = -Vab0[0] - INV_SQRT3 * Vab0[1];//Time for U1
                sector = 5;               
            }else{
                //sector 4
                t_vec1 = - 2 * INV_SQRT3 * Vab0[1];   //Time for U1
                t_vec2 = -Vab0[0] + INV_SQRT3 * Vab0[1];//Time for U3
                sector = 4; 
            }
        }
    }

    float tA, tB, tC;
    switch(sector){
        case 1:
            tC = (1.0f - t_vec1 - t_vec2) / 2.0;
            tA = tC + t_vec1 + t_vec2;
            tB = tC + t_vec2;
            break;
        case 2:
            tC = (1.0f - t_vec1 - t_vec2) / 2.0;
            tB = tC + t_vec1 + t_vec2;
            tA = tC + t_vec1;
            break;
        case 3:
            tA = (1.0f - t_vec1 - t_vec2) / 2.0;
            tB = tA + t_vec1 + t_vec2;
            tC = tA + t_vec2;
            break;
        case 4:
            tA = (1.0f - t_vec1 - t_vec2) / 2.0;
            tC = tA + t_vec1 + t_vec2;
            tB = tA + t_vec2;
            break;
        case 5:
            tB = (1.0f - t_vec1 - t_vec2) / 2.0;
            tC = tB + t_vec1 + t_vec2;
            tA = tB + t_vec1;
            break;
        case 6:
            tB = (1.0f - t_vec1 - t_vec2) / 2.0;
            tA = tB + t_vec1 + t_vec2;
            tC = tB + t_vec2;
    }

    tABC[0] = tA;
    tABC[1] = tB;
    tABC[2] = tC;
}


#endif