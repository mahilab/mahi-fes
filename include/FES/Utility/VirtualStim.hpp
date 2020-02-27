#pragma once

#include <Windows.h>
#include <string>
#include <MEL/Logging/Log.hpp>

namespace fes{
    class VirtualStim{
    private:
        DCB dcbSerialParams;
        std::string com_port;
        bool open_port();
        bool configure_port();
        HANDLE hComm;
    public:
        VirtualStim(const std::string& com_port_);
        ~VirtualStim();
        void Poll();
    };

}