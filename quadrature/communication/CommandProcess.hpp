#ifndef COMMAND_PROCESS_HPP
#define COMMAND_PROCESS_HPP

#define COMMAND_BUFFER_SIZE 64

extern "C" {
    #include <string.h>
}



extern char commandBuffer[COMMAND_BUFFER_SIZE];
extern char len;

extern bool command_updated;

void parseCommand();

extern "C" void processUSBReceive(char* buf, unsigned long* len);

#endif