#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "Axis.hpp"
#include "hardware/AS5048.hpp"
#include "hardware/TimerModulator.hpp"
#include "ControlLogic.hpp"

extern uint16_t adc_fields[3];

extern CurrentSense* axis_1_ch_A;
extern CurrentSense* axis_1_ch_B;
extern CurrentSense* axis_1_ch_C;
extern VoltageSense* pvcc_sense;

extern AS5048* axis_1_encoder;

extern Modulator* axis_1_modulator;

extern Axis* axis_1;
extern ControlLogic axis_1_control_logic;

void componentInit();

#endif