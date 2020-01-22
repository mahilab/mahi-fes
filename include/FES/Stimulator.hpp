#pragma once
#include "FES/Muscle.hpp"
namespace fes{
    class Stimulator {
    public:
        

        Stimulator(std::string& name, std::string& com_port);

        int on_enable();

        bool open_port();

        int open_port0(void);

        int open_port1(void);

        int configure_port(int fd);

        void set_stim(int pulse_width_);

        int Stimulator::checksum(unsigned char myarray[], int m); // checksum is a function that preforms checksums of all of the byte strings used in this code
        
        int Board_Setup_0(int fd);

    private:
        HANDLE hComm;
        std::string& name_;
        std::string& com_port_;
        bool open_;
    };
}