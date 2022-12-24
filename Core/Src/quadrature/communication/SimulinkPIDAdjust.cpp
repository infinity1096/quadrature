#include "SimulinkPIDAdjust.hpp"
#include "components.hpp"

bool simulink_command_updated = false;

char simulinkCommandBuffer[sizeof(SimulinkCommandPacket) + 2 * sizeof(float32_t)];
SimulinkCommandPacket simulinkCommand;

void parseSimulinkCommand(){
    if (!simulink_command_updated){
        return;
    }

    simulink_command_updated = false;

    if (strncmp((const char*)simulinkCommandBuffer, "SSSS", 4) != 0){
        return;
    }

    if (strncmp((const char*)simulinkCommandBuffer + sizeof(SimulinkCommandPacket) + sizeof(float32_t), "EEEE", 4) != 0){
        return;
    }

    // validation success, apply received signals
    memcpy(&simulinkCommand, simulinkCommandBuffer + sizeof(float32_t), sizeof(SimulinkCommandPacket));

    axis_1_control_logic.Id_Kp = simulinkCommand.PID_ID_KP;
    axis_1_control_logic.Id_Ki = simulinkCommand.PID_ID_KI;

    axis_1_control_logic.Iq_Kp = simulinkCommand.PID_IQ_KP;
    axis_1_control_logic.Iq_Ki = simulinkCommand.PID_IQ_KI;
    
    axis_1_control_logic.Id_target = simulinkCommand.ID_Target;
    axis_1_control_logic.Iq_target = simulinkCommand.IQ_Target;

    axis_1_control_logic.state_estimator.Id_complementry_gain = simulinkCommand.ID_complementary;
    axis_1_control_logic.state_estimator.Iq_complementry_gain = simulinkCommand.IQ_complementary;

    axis_1_control_logic.axis->axis_config.encoder_phase_offset = simulinkCommand.electrical_phase_offset;

    axis_1_control_logic.chirp_freq = simulinkCommand.chirp_freq;
}


extern "C" void processUSBReceive(char* buf, unsigned long* len){
    if (simulink_command_updated){
        return;
    }

    simulink_command_updated = true;

    if (*len == sizeof(SimulinkCommandPacket) + 2 * sizeof(float)){
        memcpy(simulinkCommandBuffer, buf, sizeof(SimulinkCommandPacket) + 2 * sizeof(float));
    }
}