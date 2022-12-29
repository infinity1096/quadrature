#include "Telemetry.hpp"
#include "simulink_reporter.hpp"

CurrentPacket currentPacket;
VelocityPacket velocityPacket;
PositionPacket positionPacket;

uint32_t current_loop_index = 0;
uint32_t velocity_loop_index = 0;
uint32_t position_loop_index = 0;

bool current_telemetry_enabled = false;
bool velocity_telemetry_enabled = false;
bool position_telemetry_enabled = false;


int16_t current_divisor = 1;
int16_t velocity_divisor = 1;
int16_t position_divisor = 1;

SimulinkReport<CurrentPacket, CURRENT_PACKET_LEN> current_reporter(CURRENT_PACKET_HEADER, CURRENT_PACKET_ENDING);
SimulinkReport<VelocityPacket, VELOCITY_PACKET_LEN> velocity_reporter(VELOCITY_PACKET_HEADER, VELOCITY_PACKET_ENDING);
SimulinkReport<PositionPacket, POSITION_PACKET_LEN> position_reporter(POSITION_PACKET_HEADER, POSITION_PACKET_ENDING);

void recordCurrentPacket(){
    
    if (current_telemetry_enabled && current_loop_index % current_divisor == 0){
        // record current packet
        axis_1_control_logic.state_estimator.getDQCurrent(&currentPacket.Id, &currentPacket.Iq);
        currentPacket.Vd = axis_1_control_logic.Vd_output;
        currentPacket.Vq = axis_1_control_logic.Vq_output;
        currentPacket.Iq_target = axis_1_control_logic.Iq_target;

        current_reporter.record(currentPacket);
    }

    current_loop_index++;
    if(current_loop_index == CURRENT_PACKET_PERIOD){
        current_loop_index = 0;
    }
}

void recordVelocityPacket(){
    
    if (velocity_telemetry_enabled && velocity_loop_index % velocity_divisor == 0){
        // record velocity packet
        velocityPacket.vel_mech = axis_1_control_logic.state_estimator.getVelocity();
        velocityPacket.vel_dot = axis_1_control_logic.velocity_controller.error_dot;
        velocityPacket.vel_target = axis_1_control_logic.velocity_target;
        velocityPacket.Iq_target = axis_1_control_logic.Iq_target;
        
        velocity_reporter.record(velocityPacket);
    }

    velocity_loop_index++;
    if(velocity_loop_index == VELOCITY_PACKET_PERIOD){
        velocity_loop_index = 0;
    }
}

void recordPositionPacket(){
    
    if (position_telemetry_enabled && position_loop_index % position_divisor == 0){
        // record velocity packet
        positionPacket.pos_mech = axis_1_control_logic.state_estimator.getAngle();
        positionPacket.pos_dot = axis_1_control_logic.position_controller.error_dot;
        positionPacket.pos_target = axis_1_control_logic.position_target;
        positionPacket.vel_target = axis_1_control_logic.velocity_target;

        position_reporter.record(positionPacket);
    }

    position_loop_index++;
    if(position_loop_index == POSITION_PACKET_PERIOD){
        position_loop_index = 0;
    }
}

void setTelemetryDivisor(bool current, bool velocity, bool position, float32_t divisor){
    
    if (divisor > 1){
        current_telemetry_enabled = current;
        velocity_telemetry_enabled = velocity;
        position_telemetry_enabled = position;

        current_divisor = (int16_t) divisor;
        velocity_divisor = (int16_t) divisor;
        position_divisor = (int16_t) divisor;
    }
    
}

void checkTelemetryTransmit(){
    current_reporter.checkTransmit();
    velocity_reporter.checkTransmit();
    position_reporter.checkTransmit();
}