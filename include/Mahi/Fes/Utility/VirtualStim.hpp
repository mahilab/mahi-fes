// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Nathan Dunkelberger (nbd2@rice.edu)

#pragma once

#include <Windows.h>

#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <string>
#include <thread>

namespace mahi {
namespace fes {
/// Virtual stimulator, designed to test the application of the stimulator class.
/// This creates a gui which shows the incoming messages from the stimulator 
/// separated by messages type.
class VirtualStim : public mahi::gui::Application {
public:
    /// VirtualStim Constructor
    VirtualStim(const std::string& com_port_);
    /// VirtualStim Destructor
    ~VirtualStim();
    /// continously updates all of the different messages handled by add_monitor
    /// based on inputs from the comport
    void poll();
    /// run the update to refresh the display of the application window
    void update();

private:
    struct SerialMessage {
        std::vector<unsigned char> message;
        std::string                message_type;
        double                     time    = 0.0;
        unsigned int               msg_num = 0;
    };

    /// opens the serial port as indicated by the input argument
    bool open_port();
    /// configures the comport based on general settings
    bool configure_port();
    /// function to format the message into a way that outputs nicely
    std::vector<std::string> fmt_msg(std::vector<unsigned char> message);
    /// adds a "monitor" for a specific type of message class to the gui
    void add_monitor(SerialMessage ser_msg);

    unsigned int                          m_msg_count = 0;    // total number of messages received
    DCB                                   m_dcbSerialParams;  // parameters to handle serial port communication
    std::string                           m_com_port;         // comport number - should be formatted COMX or COMXX
    HANDLE                                m_hComm;            // serial handle to the desired comport
    bool                                  m_open  = true;     // whether or not the application is open
    bool                                  m_pause = false;    // pauses the recent messages feed
    std::thread                           m_poll_thread;      // thread for handling continuous polling
    mahi::util::RingBuffer<SerialMessage> m_recent_messages;  // ring buffer of recent messages
    std::vector<SerialMessage>            m_recent_feed;      // vector of recent messages from the recent_messages ringbuffer
    std::string                           m_recent_msg_uc;    // recent message as an unsigned char
    std::string                           m_recent_msg_int;   // recent message as an int

    // different formats of SerialMessages
    SerialMessage m_recent_message           = {std::vector<unsigned char>{}, "Recent Message", 0.0, 0};
    SerialMessage m_channel_setup_message    = {std::vector<unsigned char>{}, "Channel Setup", 0.0, 0};
    SerialMessage m_scheduler_setup_message  = {std::vector<unsigned char>{}, "Scheduler Setup", 0.0, 0};
    SerialMessage m_scheduler_halt_message   = {std::vector<unsigned char>{}, "Halt Scheduler", 0.0, 0};
    SerialMessage m_scheduler_delete_message = {std::vector<unsigned char>{}, "Delete Scheduler", 0.0, 0};
    SerialMessage m_scheduler_sync_message   = {std::vector<unsigned char>{}, "Sync Scheduler", 0.0, 0};
    SerialMessage m_event_create_message     = {std::vector<unsigned char>{}, "Create Event", 0.0, 0};
    SerialMessage m_event_delete_message     = {std::vector<unsigned char>{}, "Delete Event", 0.0, 0};
    SerialMessage m_event_edit_1_message     = {std::vector<unsigned char>{}, "Edit Event 1", 0.0, 0};
    SerialMessage m_event_edit_2_message     = {std::vector<unsigned char>{}, "Edit Event 2", 0.0, 0};
    SerialMessage m_event_edit_3_message     = {std::vector<unsigned char>{}, "Edit Event 3", 0.0, 0};
    SerialMessage m_event_edit_4_message     = {std::vector<unsigned char>{}, "Edit Event 4", 0.0, 0};
    SerialMessage m_unknown_message          = {std::vector<unsigned char>{}, "Unkown", 0.0, 0};
};

}  // namespace fes
}  // namespace mahi