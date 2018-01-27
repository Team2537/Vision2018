#ifndef __RIO_SERIAL__
#define __RIO_SERIAL__

extern "C"{
    #include <wiringPi.h>
    #include <wiringSerial.h>
}

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#define SERIAL_PORT "/dev/serial0"
#define BAUDRATE 38400

class RioSerial {

    public:

        RioSerial(RioSerial const&) = delete;
        void operator=(RioSerial const&) = delete;

        static int open();
        static void close();

        static void write(std::string message);
        static std::string read();

    protected:

        RioSerial();

        static void *processSerial(void* args);

        static pthread_t thread;

        static bool keepAlive;

        static std::string outputBuffer;
        static std::string inputBuffer;
};

#endif /* __RIO_SERIAL__ */
