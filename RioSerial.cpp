#include "RioSerial.hpp"

pthread_t RioSerial::thread;
bool RioSerial::keepAlive = true;
std::string RioSerial::outputBuffer;
std::string RioSerial::inputBuffer;

int RioSerial::open(){
    return pthread_create(&thread, NULL, processSerial, (void*) NULL);
}

void RioSerial::write(std::string message){
    outputBuffer += message;
}

std::string RioSerial::read(){
    std::string returnString = inputBuffer;
    inputBuffer = "";
    return returnString;
}

void* RioSerial::processSerial(void* args){
    int serialId;
    if((serialId = serialOpen(SERIAL_PORT, BAUDRATE)) < 0){
        std::cerr << "Unable to open serial communications: " << SERIAL_PORT << std::endl;
        pthread_exit((void*)-1);
    }

    if(wiringPiSetup() == -1){
        std::cerr << "wiringPiSetup failed" << std::endl;
        pthread_exit((void*)-1);
    }

    while(keepAlive){
        if(outputBuffer != ""){
            serialPuts(serialId, outputBuffer.c_str());
            outputBuffer = "";
        }
        for(int x = 0; x < serialDataAvail(serialId); x++){
            inputBuffer += serialGetchar(serialId);
        }
    }
}
