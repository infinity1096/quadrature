#ifndef COMMAND_INTERFACE
#define COMMAND_INTERFACE

#include <string>

bool armAxis();
bool disarmAxis();
bool setParameter(std::string name, std::string value);
bool saveConfig();
bool dumpParameters();
bool setCurrent(float value);
bool setTorque(float value);
bool setVelocity(float value);
bool setPosition(float value);
bool setTelemetry(bool enabled);

#endif