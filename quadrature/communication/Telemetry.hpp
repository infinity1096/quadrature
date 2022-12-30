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

#define CURRENT_PACKET_HEADER "CCCC"
#define CURRENT_PACKET_ENDING "DDDD"
#define VELOCITY_PACKET_HEADER "VVVV"
#define VELOCITY_PACKET_ENDING "WWWW"
#define POSITION_PACKET_HEADER "PPPP"
#define POSITION_PACKET_ENDING "QQQQ"

#define CURRENT_PACKET_PERIOD 24000
#define VELOCITY_PACKET_PERIOD 10000
#define POSITION_PACKET_PERIOD 10000

#define CURRENT_PACKET_LEN 100
#define VELOCITY_PACKET_LEN 20
#define POSITION_PACKET_LEN 20

struct CurrentPacket {
    float32_t Ialpha;
    float32_t Ibeta;
    float32_t Valpha;
    float32_t Vbeta;
    float32_t theta_e;
};

struct VelocityPacket {
    float32_t vel_mech;
    float32_t vel_dot;
    float32_t Iq_target;
    float32_t vel_target;
};

struct PositionPacket {
    float32_t pos_mech;
    float32_t pos_dot;
    float32_t vel_target;
    float32_t pos_target;
};

void recordCurrentPacket();
void recordVelocityPacket();
void recordPositionPacket();

void setTelemetryDivisor(bool current, bool velocity, bool position, float32_t divisor);
void checkTelemetryTransmit();

#endif