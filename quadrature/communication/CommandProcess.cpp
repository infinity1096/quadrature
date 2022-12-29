#include "CommandProcess.hpp"
#include "components.hpp"
#include "Telemetry.hpp"

bool command_updated = false;

char commandBuffer[COMMAND_BUFFER_SIZE];
int dataLength = 0;

void executeSetTarget(const SetTargetCommand& command);
void executeSetPID(const SetPIDCommand& command);
void executeSetTelemetry(const SetTelemetryCommand& command);

void parseCommand(){
    if (!command_updated){
        return;
    }

    switch (dataLength){
        case sizeof(SetTargetCommand):
            SetTargetCommand command1;
            memcpy(&command1, commandBuffer, dataLength);

            if (strncpy((char*)&command1.header, SET_TARGET_HEADER, 4) != 0){
                break;
            }

            if (strncpy((char*)&command1.ending, SET_TARGET_ENDING, 4) != 0){
                break;
            }

            // parse success, execute command
            executeSetTarget(command1);

            break;
        case sizeof(SetPIDCommand):
            SetPIDCommand command2;
            memcpy(&command2, commandBuffer, dataLength);

            if (strncpy((char*)&command2.header, SET_PID_HEADER, 4) != 0){
                break;
            }

            if (strncpy((char*)&command2.ending, SET_PID_ENDING, 4) != 0){
                break;
            }

            // parse success, execute command
            executeSetPID(command2);

            break;
        case sizeof(SetTelemetryCommand):
            SetTelemetryCommand command3;
            memcpy(&command3, commandBuffer, dataLength);

            if (strncpy((char*)&command3.header, SET_TELEMETRY_HEADER, 4) != 0){
                break;
            }

            if (strncpy((char*)&command3.ending, SET_TELEMETRY_HEADER, 4) != 0){
                break;
            }

            // parse success, execute command
            executeSetTelemetry(command3);

            break;
    }

    command_updated = false;
}

void executeSetTarget(const SetTargetCommand& command){
    
    // process arming command
    if (command.arm == 0.0f){
        axis_1->disarm();
    }else if (command.arm == 1.0f){
        axis_1->requestArm();
    }

    // process control mode and target
    if (command.control_mode == 0.0f){
        // control mode = NONE
        axis_1_control_logic.control_mode = ControlMode::NONE;
    
    } else if (command.control_mode == 1.0f){
        // control mode = CURRENT
        axis_1_control_logic.control_mode = ControlMode::CURRENT;
        axis_1_control_logic.Iq_target = clip(command.target, -axis_1->axis_config.current_limit, axis_1->axis_config.current_limit);

    } else if (command.control_mode == 2.0f){
        // control mode = VELOCITY
        axis_1_control_logic.control_mode = ControlMode::VELOCITY;
        axis_1_control_logic.velocity_target = clip(command.target, -axis_1->axis_config.velocity_limit, axis_1->axis_config.velocity_limit);
    
    } else if (command.control_mode == 3.0f){
        // control mode = POSITION
        axis_1_control_logic.control_mode = ControlMode::POSITION;
        axis_1_control_logic.position_target = command.target;
    
    }

}

void executeSetPID(const SetPIDCommand& command){
    
    if (command.pid_index == 0.0f || command.pid_index == 1.0f){
        // PID index is D current loop or Q current loop
        PIDController<false>* pid_ptr = nullptr;

        if (command.pid_index == 0.0f){
            pid_ptr = &axis_1_control_logic.Id_controller;
        }else{
            pid_ptr = &axis_1_control_logic.Iq_controller;
        }

        // set parameters
        pid_ptr->Kp = command.kp;
        pid_ptr->Ki = command.ki;
        pid_ptr->back_calculation_coeff = command.back_calc;

    } else if (command.pid_index == 2.0f || command.pid_index == 3.0f){
        // PID index is velocity / position loop
        PIDController<true>* pid_ptr = nullptr;

        if (command.pid_index == 2.0f){
            pid_ptr = &axis_1_control_logic.velocity_controller;
        }else{
            pid_ptr = &axis_1_control_logic.position_controller;
        }

        // set parameters
        pid_ptr->Kp = command.kp;
        pid_ptr->Ki = command.ki;
        pid_ptr->Kd = command.kd;
        pid_ptr->N = command.N;
        pid_ptr->back_calculation_coeff = command.back_calc;

    }

}

void executeSetTelemetry(const SetTelemetryCommand& command){
    setTelemetryDivisor(command.current_info_on, command.velocity_info_on, command.position_info_on, command.rate_divisor);
}

extern "C" void processUSBReceive(char* buf, unsigned long* len){
    if (command_updated){
        return;
    }

    if (*len <= COMMAND_BUFFER_SIZE-1){
        memcpy(commandBuffer, buf, *len);
        dataLength = *len;
        commandBuffer[*len] = '\0';
        command_updated = true;
    }
}