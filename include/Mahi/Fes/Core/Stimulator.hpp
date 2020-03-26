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

#include <Mahi/Fes/Core/Channel.hpp>
#include <Mahi/Fes/Core/ReadMessage.hpp>
#include <Mahi/Fes/Core/Scheduler.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace mahi {
namespace fes {
class Stimulator {
public:
    /// Stimulator constructor
    Stimulator(const std::string& name_, const std::string& com_port_, std::vector<Channel>& channels_, size_t size_, bool is_virtual_ = false);
    /// Stimulator destructor
    ~Stimulator();
    /// open, configure, and initialize the serial communication for use with the board
    bool enable();
    /// disable all schedulers and events and close serial communication
    void disable();
    /// takes the scheduler objet that already exists and sends the create_scheduler message
    /// to the UECU, assigning scheduler id in the process
    bool create_scheduler(const unsigned char sync_msg, double frequency_);
    /// checks whether the stimulator has been enabled
    bool is_enabled();
    /// set the amplitude for a single channel (event). This runs down to the event object
    void set_amp(Channel channel_, unsigned int amplitude_);
    /// set the amplitude for a vector of channels (events). This runs down to the event object
    void set_amps(std::vector<Channel> channels_, std::vector<int> amplitudes_);
    /// set the pulsewidth for a single channel (event). This runs down to the event object
    void write_pw(Channel channel_, unsigned int pw_);
    /// set the pulsewidth for a vector of channels (events). This runs down to the event object
    void write_pws(std::vector<Channel> channels_, std::vector<int> pulsewidths_);
    /// update the max amplitude for a single channel. This runs down to the event object
    void update_max_amp(Channel channel_, unsigned int max_amp_);
    /// update the max pulsewidth for a single channel. This runs down to the event object
    void update_max_pw(Channel channel_, unsigned int max_pw_);
    /// add an event to the scheduler
    bool add_event(Channel channel_, unsigned char event_type = STIM_EVENT);
    /// add a vector of events to the scheduler
    bool add_events(std::vector<Channel> channels_, unsigned char event_type = STIM_EVENT);
    /// return a vector of the channels of the stimulator
    std::vector<Channel> get_channels();
    /// start the stimulator by sending the sync message
    bool begin();
    /// command values set by set_amp/pw commands by sending messages to the UECU
    bool update();
    /// halt the scheduler, cancelling all events and schedulers
    bool halt_scheduler();
    /// return the name of the stimulator
    std::string get_name();

    size_t                   num_events;       // number of events the stimulator is handling
    std::vector<int>         amplitudes;       // vector of amplitudes corresponding to channels
    std::vector<int>         pulsewidths;      // vector of pulsewidths corresponding to channels
    std::vector<int>         max_amplitudes;   // vector of max amplitudes corresponding to channels
    std::vector<int>         max_pulsewidths;  // vector of max pulsewidths corresponding to channels
    std::vector<std::string> channel_names;    // returns vector of the names of the channels

private:
    /// open the comport that the UECU is controlled from
    bool open_port();
    /// configure the comport that the UECU is controlled from
    bool configure_port();
    /// initialize the board by enabling each of the channels given setup parameters
    bool initialize_board();
    /// halt the stimulator and close the comports
    void close_stimulator();
    /// read all incoming messages from the stimulator
    void read_all();

    HANDLE m_hComm;                  // serial handle to the appropriate UECU
    DCB    m_dcbSerialParams = {0};  // serial parameters to handle the serial communication to UECU

    mahi::util::Time m_delay_time = mahi::util::milliseconds(100);  // delay time when sending messages

    std::string             m_name;               // name of the stimulator
    std::string             m_com_port;           // comport that the UECU is written to from. should be in format COMX or COMXX
    bool                    m_enabled;            // shows if the stimulator has been enabled
    bool                    m_is_virtual;         // determines whether or not to wait for responses from the stimulator
    std::vector<Channel>    m_channels;           // vector of channels enabled by the stim board
    Scheduler               m_scheduler;          // scheduler which handles events
    int                     m_inc_msg_count = 0;  // number of messages the stimulator has received
    std::queue<ReadMessage> m_inc_messages;       // queue of incoming messages
    std::mutex              m_mtx;                // mutex for handling simultaneous reading/writing
};
}  // namespace fes
}  // namespace mahi