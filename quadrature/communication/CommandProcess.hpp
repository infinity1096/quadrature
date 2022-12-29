#ifndef COMMAND_PROCESS_HPP
#define COMMAND_PROCESS_HPP

#define COMMAND_BUFFER_SIZE 64

extern "C" {
    #include <string.h>
    #include "arm_math.h"
}

#define SET_TARGET_HEADER       "AAAA"
#define SET_TARGET_ENDING       "BBBB"
#define SET_PID_HEADER          "CCCC"
#define SET_PID_ENDING          "DDDD"
#define SET_TELEMETRY_HEADER    "EEEE"
#define SET_TELEMETRY_ENDING    "FFFF"

struct SetTargetCommand {
    uint32_t header;
    float32_t arm;
    float32_t control_mode;
    float32_t target;
    uint32_t ending;
};

struct SetPIDCommand {
    uint32_t header;
    float32_t pid_index;
    float32_t kp;
    float32_t ki;
    float32_t kd;
    float32_t N;
    float32_t back_calc;
    uint32_t ending;
};

struct SetTelemetryCommand {
    uint32_t header;
    float32_t current_info_on;
    float32_t velocity_info_on;
    float32_t position_info_on;
    float32_t rate_divisor;
    uint32_t ending;
};

extern char commandBuffer[COMMAND_BUFFER_SIZE];
extern char len;

extern bool command_updated;

void parseCommand();

extern "C" void processUSBReceive(char* buf, unsigned long* len);

#endif