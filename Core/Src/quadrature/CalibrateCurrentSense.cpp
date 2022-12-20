#include "CalibrateCurrentSense.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"

#ifdef __cplusplus
}
#endif

CalibrateCurrentSense::CalibrateCurrentSense(CurrentSense* currentSense, long numSample) {
    num_sample = numSample;
    accumlated_sum = 0;
    accumlated_idx = 0;
    current_sense = currentSense;
}

void CalibrateCurrentSense::initialize() {
    accumlated_sum = 0;
    accumlated_idx = 0;
}

void CalibrateCurrentSense::execute() {
    accumlated_idx++;
    accumlated_sum += current_sense->sensed_current;
}

bool CalibrateCurrentSense::isFinished() {
    return accumlated_idx == num_sample;
}

void CalibrateCurrentSense::end() {
    float avgCurrent = accumlated_sum / accumlated_idx;
    float avgVoltage = current_sense->convertCurrentToVoltage(avgCurrent);
    current_sense->config.amplifier_baseline = avgVoltage;
}