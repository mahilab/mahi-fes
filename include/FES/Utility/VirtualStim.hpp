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
        struct SerialMessage{
            std::vector<unsigned char> message;
            std::string message_type;
            float time = 0.0;
        };
        DCB dcbSerialParams;
        std::string com_port;
        bool open_port();
        bool configure_port();
        void add_monitor(SerialMessage ser_msg);
        HANDLE hComm;
        bool open = true;
        std::thread poll_thread;
        std::thread run_thread;
        std::string recent_msg_uc;
        std::string recent_msg_int;
        SerialMessage recent_message = {std::vector<unsigned char> {}, "Recent Message", 0.0};
        SerialMessage channel_setup_message = {std::vector<unsigned char> {}, "Channel Setup", 0.0};
        SerialMessage scheduler_setup_message = {std::vector<unsigned char> {}, "Scheduler Setup", 0.0};
        SerialMessage scheduler_sync_message = {std::vector<unsigned char> {}, "Sync Scheduler", 0.0};
        SerialMessage event_create_message = {std::vector<unsigned char> {}, "Create Event", 0.0};
        SerialMessage event_edit_1_message = {std::vector<unsigned char> {}, "Edit Event 1", 0.0};
        SerialMessage event_edit_2_message = {std::vector<unsigned char> {}, "Edit Event 2", 0.0};
        SerialMessage event_edit_3_message = {std::vector<unsigned char> {}, "Edit Event 3", 0.0};
        SerialMessage event_edit_4_message = {std::vector<unsigned char> {}, "Edit Event 4", 0.0};
        SerialMessage unknown_message = {std::vector<unsigned char> {}, "Unkown", 0.0};
        // std::thread poll_thread;
        
    public:
        VirtualStim(const std::string& com_port_);
        ~VirtualStim();
        void poll();
        void update();
        void begin();
    };

}