#ifndef SIMULINK_PID_ADJUST
#define SIMULINK_PID_ADJUST

extern "C" {
    #include <string.h>
}

struct SimulinkCommandPacket {
    float PID_ID_KP;
    float PID_ID_KI;
    float ID_complementary;
    float PID_IQ_KP;
    float PID_IQ_KI;
    float IQ_complementary;
    float ID_Target;
    float IQ_Target;
    float electrical_phase_offset;
    float chirp_freq;
};

extern char simulinkCommandBuffer[sizeof(SimulinkCommandPacket) + 2 * sizeof(float)];
extern SimulinkCommandPacket simulinkCommand;

extern bool simulink_command_updated;

void parseSimulinkCommand();

extern "C" void processUSBReceive(char* buf, unsigned long* len);

#endif