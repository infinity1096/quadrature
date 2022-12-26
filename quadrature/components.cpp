#include "components.hpp"
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"

#ifdef __cplusplus
}
#endif

uint16_t adc_fields[3];
uint16_t volt_adc;

CurrentSense* axis_1_ch_A = nullptr;
CurrentSense* axis_1_ch_B = nullptr;
CurrentSense* axis_1_ch_C = nullptr;
VoltageSense* pvcc_sense = nullptr;

AS5048* axis_1_encoder = nullptr;

Modulator* axis_1_modulator = nullptr;

Axis* axis_1;

ControlLogic axis_1_control_logic;

std::map<std::string, float*> field_map;
void initFieldMap();
void componentInit(){
    
    VoltageSenseConfig unitVoltageSense;
    CurrentSenseConfig shuntCurrentSense;

    VoltageSenseConfig pvccSenseConfig;
    pvccSenseConfig.resistor_bridge_ratio = 1.0/11.0;


    axis_1_ch_A = new CurrentSense(adc_fields + 0, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_B = new CurrentSense(adc_fields + 1, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_C = new CurrentSense(adc_fields + 2, unitVoltageSense, shuntCurrentSense);
    
    pvcc_sense = new VoltageSense(&volt_adc, pvccSenseConfig);

    axis_1_modulator = new TimerModulator(&htim1);
    axis_1_modulator->initialize();

    axis_1_encoder = new AS5048(&hspi3, GPIOB, GPIO_PIN_6);

    if (!axis_1_encoder->initialize()){
        while (1) {}
    }

    AxisConfig axis_config;

    axis_config.axis_direction = Direction::NORMAL;
    axis_config.mechanical_offset = 0;
    axis_config.encoder_direction = Direction::INVERTED;
    axis_config.encoder_phase_offset = 0.27;
    axis_config.encoder_calibrated = true;
    axis_config.pole_pairs = 21;
    axis_config.phase_resistance = 0.1;
    axis_config.phase_inductance = 0.36e-3;
    axis_config.torque_constant = 0.056172;
    axis_config.motor_calibrated = true;
    
    axis_1 = new Axis(axis_config);

    axis_1->attachEncoder(axis_1_encoder);
    axis_1->attachModulator(axis_1_modulator);

    axis_1->attachCurrentSense(0, axis_1_ch_A);
    axis_1->attachCurrentSense(1, axis_1_ch_B);
    axis_1->attachCurrentSense(2, axis_1_ch_C);

    axis_1->attachVoltageSense(pvcc_sense);

    axis_1_control_logic.setAxis(axis_1);

    axis_1_control_logic.Id_controller.Kp = 0.446f;
    axis_1_control_logic.Id_controller.Ki = 812.0f;
    axis_1_control_logic.Id_controller.back_calculation_coeff = 1.0;
    
    axis_1_control_logic.Iq_controller.Kp = 0.45f;
    axis_1_control_logic.Iq_controller.Ki = 1200.0f;
    axis_1_control_logic.Iq_controller.back_calculation_coeff = 1.0;
    
    axis_1_control_logic.Id_controller.reset();
    axis_1_control_logic.Iq_controller.reset();

    // build component tree
    initFieldMap();

    // start ADC read
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(adc_fields), 3);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&volt_adc, 1);

    // start encoder Timer
    HAL_TIM_Base_Start_IT(&htim2);
}

void initFieldMap(){
    field_map["id_kp"] = &axis_1_control_logic.Id_controller.Kp;
    field_map["id_ki"] = &axis_1_control_logic.Id_controller.Ki;
    field_map["id_bc"] = &axis_1_control_logic.Id_controller.back_calculation_coeff;
    
    field_map["iq_kp"] = &axis_1_control_logic.Iq_controller.Kp;
    field_map["iq_ki"] = &axis_1_control_logic.Iq_controller.Ki;
    field_map["iq_bc"] = &axis_1_control_logic.Iq_controller.back_calculation_coeff;
    
    field_map["v_kp"] = &axis_1_control_logic.velocity_controller.Kp;
    field_map["v_ki"] = &axis_1_control_logic.velocity_controller.Ki;
    field_map["v_kd"] = &axis_1_control_logic.velocity_controller.Kd;
    field_map["v_n"] =  &axis_1_control_logic.velocity_controller.N;
    field_map["v_bc"] = &axis_1_control_logic.velocity_controller.back_calculation_coeff;
    
    field_map["p_kp"] = &axis_1_control_logic.position_controller.Kp;
    field_map["p_ki"] = &axis_1_control_logic.position_controller.Ki;
    field_map["p_kd"] = &axis_1_control_logic.position_controller.Kd;
    field_map["p_n"] =  &axis_1_control_logic.position_controller.N;
    field_map["p_bc"] = &axis_1_control_logic.position_controller.back_calculation_coeff;
}

#if 0
void buildComponentTree(){
    fieldTreeRoot = new FieldTreeNode("axis1");

    auto IdPID = new FieldTreeNode("IdPID");
    IdPID->addChild(new AdjustableField("p", &axis_1_control_logic.Id_controller.Kp));
    IdPID->addChild(new AdjustableField("i", &axis_1_control_logic.Id_controller.Ki));
    IdPID->addChild(new AdjustableField("back_calc", &axis_1_control_logic.Id_controller.back_calculation_coeff));

    auto IqPID = new FieldTreeNode("IqPID");    
    IqPID->addChild(new AdjustableField("p", &axis_1_control_logic.Iq_controller.Kp));
    IqPID->addChild(new AdjustableField("i", &axis_1_control_logic.Iq_controller.Ki));
    IqPID->addChild(new AdjustableField("back_calc", &axis_1_control_logic.Iq_controller.back_calculation_coeff));

    auto VelPID = new FieldTreeNode("VelPID");    
    VelPID->addChild(new AdjustableField("p", &axis_1_control_logic.velocity_controller.Kp));
    VelPID->addChild(new AdjustableField("i", &axis_1_control_logic.velocity_controller.Ki));
    VelPID->addChild(new AdjustableField("d", &axis_1_control_logic.velocity_controller.Kd));
    VelPID->addChild(new AdjustableField("N", &axis_1_control_logic.velocity_controller.N));
    VelPID->addChild(new AdjustableField("back_calc", &axis_1_control_logic.velocity_controller.back_calculation_coeff));

    auto PosPID = new FieldTreeNode("PosPID");    
    PosPID->addChild(new AdjustableField("p", &axis_1_control_logic.position_controller.Kp));
    PosPID->addChild(new AdjustableField("i", &axis_1_control_logic.position_controller.Ki));
    PosPID->addChild(new AdjustableField("d", &axis_1_control_logic.position_controller.Kd));
    PosPID->addChild(new AdjustableField("N", &axis_1_control_logic.position_controller.N));
    PosPID->addChild(new AdjustableField("back_calc", &axis_1_control_logic.position_controller.back_calculation_coeff));

    fieldTreeRoot->addChild(IdPID);
    fieldTreeRoot->addChild(IqPID);
    fieldTreeRoot->addChild(VelPID);
    fieldTreeRoot->addChild(PosPID);
}
#endif