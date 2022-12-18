#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "Axis.hpp"
#include "hardware/AS5048.hpp"
#include "hardware/TimerModulator.hpp"

extern uint16_t adc_fields[3];

extern CurrentSense* axis_1_ch_A;
extern CurrentSense* axis_1_ch_B;
extern CurrentSense* axis_1_ch_C;
extern VoltageSense* pvcc_sense;

extern Encoder* axis_1_encoder;

extern Modulator* axis_1_modulator;

void componentInit();

#endif