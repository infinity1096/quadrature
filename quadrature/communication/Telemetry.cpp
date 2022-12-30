#include "Telemetry.hpp"
#include "simulink_reporter.hpp"

CurrentPacket currentPacket;
SimulinkReport<CurrentPacket, CURRENT_PACKET_LEN> current_reporter(CURRENT_PACKET_HEADER, CURRENT_PACKET_ENDING);


void checkTelemetryTransmit(){
    current_reporter.checkTransmit();
}