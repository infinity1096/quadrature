#include "quadrature/components.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"

#ifdef __cplusplus
}
#endif

uint16_t adc_fields[3];

CurrentSense* axis_1_ch_A = nullptr;
CurrentSense* axis_1_ch_B = nullptr;
CurrentSense* axis_1_ch_C = nullptr;
VoltageSense* pvcc_sense = nullptr;

AS5048* axis_1_encoder = nullptr;

Modulator* axis_1_modulator = nullptr;

Axis* axis_1;

ControlLogic axis_1_control_logic;

void componentInit(){
    
    VoltageSenseConfig unitVoltageSense;
    CurrentSenseConfig shuntCurrentSense;

    VoltageSenseConfig pvccSenseConfig;
    pvccSenseConfig.resistor_bridge_ratio = 1.0/11.0;


    axis_1_ch_A = new CurrentSense(adc_fields + 0, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_B = new CurrentSense(adc_fields + 1, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_C = new CurrentSense(adc_fields + 2, unitVoltageSense, shuntCurrentSense);
    
    // FIXME:
    uint16_t volt_adc = 0;
    pvcc_sense = new VoltageSense(&volt_adc, pvccSenseConfig);

    axis_1_modulator = new TimerModulator(&htim1);
    axis_1_modulator->initialize();

    axis_1_encoder = new AS5048(&hspi3, GPIOB, GPIO_PIN_6);

    if (!axis_1_encoder->initialize()){
        while (1) {}
    }

    AxisConfig axis_config {
        .axis_direction = Direction::NORMAL,
        .mechanical_offset = 0,
        .encoder_direction = Direction::UNKNOWN,
        .encoder_phase_offset = 0.0,
        .encoder_calibrated = false,
        .pole_pairs = 21,
        .phase_resistance = 0.1,
        .phase_inductance = 0.36e-3,
        .torque_constant = 0.056172,
        .motor_calibrated = true
    };

    axis_1 = new Axis(axis_config);

    axis_1->attachEncoder(axis_1_encoder);
    axis_1->attachModulator(axis_1_modulator);

    axis_1->attachCurrentSense(0, axis_1_ch_A);
    axis_1->attachCurrentSense(1, axis_1_ch_B);
    axis_1->attachCurrentSense(2, axis_1_ch_C);

    axis_1->attachVoltageSense(pvcc_sense);

    axis_1_control_logic.setAxis(axis_1);

    // start ADC read
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(adc_fields), 3);
    
    // start encoder Timer
    HAL_TIM_Base_Start_IT(&htim2);
}
