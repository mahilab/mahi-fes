#include <string>
#include <vector>
#include <FES/Core/Scheduler.hpp>
#include <FES/Core/Channel.hpp>


#pragma once

namespace fes{
    class Stimulator {
    public:

        Stimulator(const std::string& name_, const std::string& com_port_, std::vector<Channel>& channels_);
        
        ~Stimulator();

        bool enable();

        void set_stim(const int pulse_width_);

        std::string get_name();

        bool create_scheduler();

        bool is_enabled();

    private:
        // variables for serial communication
        HANDLE hComm;
        DCB dcbSerialParams = {0};

        std::string name;
        std::string com_port;
        bool open;
        std::vector<Channel> channels;
        Scheduler scheduler();
        Time setup_time = milliseconds(100);

        bool open_port(HANDLE& hComm_);

        bool configure_port(HANDLE& hComm_);

        int checksum(unsigned char myarray[], int m); // checksum is a function that preforms checksums of all of the byte strings used in this code
        
        bool initialize_board(HANDLE& hComm_);

        bool write_setup_message(HANDLE& handle_, unsigned char setup_message_[], std::string message_string_,const mel::Time delay_time_);
    };
    
    // Message commands for sending serial packets
    #define TRIGGER_SETUP_MSG           0x03
    #define HALT_MSG                    0x04
    #define ERROR_REPORT_MSG            0x05
    #define EVENT_ERROR_MSG             0x06
    #define CREATE_SCHEDULE_MSG         0x10
    #define CREATE_SCHEDULE_REPLY_MSG   0x11
    #define DELETE_SCHEDULE_MSG         0x12
    #define CHANGE_SCHEDULE_MSG         0x13
    #define CHANGE_SCHEDULE_STATE_MSG   0x14
    #define CREATE_EVENT_MSG            0x15
    #define CREATE_EVENT_REPLY_MSG      0x16
    #define DELETE_EVENT_MSG            0x17
    #define CHANGE_EVENT_SCHED_MSG      0x18
    #define CHANGE_EVENT_PARAMS_MSG     0x19
    #define SYNC_MSG                    0x1B
    #define EVENT_COMMAND_MSG           0x1C
    #define CHANNEL_SETUP_MSG           0x47
    #define EVENT_COMMAND_REPLY         0x49

    // Predefined destination and sources addresses to be used for all communication
    #define DEST_ADR                    0x04
    #define SRC_ADR                     0x80

    // Length of messages which should never be changed
    #define SYNC_MSG_LEN                0x01
    #define CREATE_SCHED_LEN            0x03
    #define CH_SET_LEN                  0x07
    #define CR_EVT_LEN                  0x09

    // Anode Cathode pairs for channels 1-4
    #define AN_CA_1                     0x01
    #define AN_CA_2                     0x23
    #define AN_CA_3                     0x45
    #define AN_CA_4                     0x67
}