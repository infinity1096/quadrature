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

void componentInit(){
    
    VoltageSenseConfig unitVoltageSense;
    CurrentSenseConfig shuntCurrentSense;

    VoltageSenseConfig pvccSenseConfig;
    pvccSenseConfig.resistor_bridge_ratio = 1.0/11.0;


    axis_1_ch_A = new CurrentSense(adc_fields + 0, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_B = new CurrentSense(adc_fields + 1, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_C = new CurrentSense(adc_fields + 2, unitVoltageSense, shuntCurrentSense);
    
    axis_1_modulator = new TimerModulator(&htim1);
    axis_1_modulator->initialize();

    axis_1_encoder = new AS5048(&hspi3, GPIOB, GPIO_PIN_6);

    if (!axis_1_encoder->initialize()){
        while (1) {}
    }

    // start ADC read
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(adc_fields), 3);
    
    // start encoder Timer
    HAL_TIM_Base_Start_IT(&htim2);
}
