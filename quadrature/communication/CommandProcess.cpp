#include "CommandProcess.hpp"
#include "components.hpp"

#include "Statement.hpp"

bool command_updated = false;

char commandBuffer[COMMAND_BUFFER_SIZE];
int dataLength = 0;

void parseCommand(){
    if (!command_updated){
        return;
    }

    command_updated = false;

    Statement* statement = nullptr;
    switch (commandBuffer[0]){
        case 'a':   statement = new ArmAxisStatement();         break;
        case 'd':   statement = new DisarmAxisStatement();      break;
        case 's':   statement = new SetParameterStatement();    break;
        case 'f':   statement = new FlashSaveStatement();       break;
        case 'l':   statement = new ListParametersStatement();  break;
        case 'c':   statement = new SetCurrentStatement();      break;
        case 't':   statement = new SetTorqueStatement();       break;
        case 'v':   statement = new SetVelocityStatement();     break;
        case 'p':   statement = new SetPositionStatement();     break;
        case 'i':                                               break; 
    }

    if (statement != nullptr && statement->recognize(commandBuffer, dataLength) && statement->parse(commandBuffer, dataLength)){
        statement->execute();
    }

    if (statement != nullptr){
        delete statement;
    }
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