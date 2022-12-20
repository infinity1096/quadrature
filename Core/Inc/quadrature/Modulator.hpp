#ifndef MODULATOR_HPP
#define MODULATOR_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"

#ifdef __cplusplus
}
#endif

#include "safety.hpp"

class Modulator : public SafetyComponent {
    public:
    virtual void initialize()=0;

    virtual void hardwareEnable()=0;
    virtual void hardwareDisable()=0;

    bool isFault(){ return fault; }
    
    virtual void modulate(float32_t Va, float32_t Vb, float32_t Vc, float32_t Vdc)=0;
    virtual void modulate(float32_t Vab0[3], float32_t Vdc)=0;

    private:
    bool fault = false;
    bool enabled = false;
};

#endif