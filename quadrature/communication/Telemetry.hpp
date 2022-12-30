#ifndef TELEMETRY_HPP
#define TELEMETRY_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"

#ifdef __cplusplus
}
#endif

#include "components.hpp"
#include "simulink_reporter.hpp"

#define CURRENT_PACKET_HEADER "CCCC"
#define CURRENT_PACKET_ENDING "DDDD"
#define VELOCITY_PACKET_HEADER "VVVV"
#define VELOCITY_PACKET_ENDING "WWWW"
#define POSITION_PACKET_HEADER "PPPP"
#define POSITION_PACKET_ENDING "QQQQ"

#define CURRENT_PACKET_PERIOD 24000
#define VELOCITY_PACKET_PERIOD 10000
#define POSITION_PACKET_PERIOD 10000

#define CURRENT_PACKET_LEN 200

struct CurrentPacket {
    float32_t Ialpha;
    float32_t Ibeta;
    float32_t Valpha;
    float32_t Vbeta;
    float32_t theta_e;
};

extern SimulinkReport<CurrentPacket, CURRENT_PACKET_LEN> current_reporter;
extern CurrentPacket currentPacket;
void checkTelemetryTransmit();

#endif