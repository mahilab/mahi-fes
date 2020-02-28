#pragma once

#include <Windows.h>
#include <string>
#include <MEL/Logging/Log.hpp>
#include <mahi/gui.hpp>
#include <thread>

using namespace mahi::gui;

namespace fes{
    class VirtualStim : public Application{
    private:
        DCB dcbSerialParams;
        std::string com_port;
        bool open_port();
        bool configure_port();
        HANDLE hComm;
        bool open = true;
        std::thread poll_thread;
        std::string recent_msg_uc;
        std::string recent_msg_int;
    public:
        VirtualStim(const std::string& com_port_);
        ~VirtualStim();
        void poll();
        void update();
    };

}