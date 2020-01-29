#pragma once
#include "FES/Muscle.hpp"

namespace fes{
    class Stimulator {
    public:

        Stimulator(std::string& name, std::string& com_port);

        bool enable();

        bool open_port(HANDLE& hComm_);

        // int open_port0(void);

        // int open_port1(void);

        bool configure_port(HANDLE& hComm_);

        void set_stim(int pulse_width_);

        int checksum(unsigned char myarray[], int m); // checksum is a function that preforms checksums of all of the byte strings used in this code
        
        bool initialize_board(HANDLE& hComm_);

        std::string get_name();

    private:
        // variables for serial communication
        HANDLE hComm;
        DCB dcbSerialParams = {0};

        std::string& name_;
        std::string& com_port_;
        bool open_;
    };
}