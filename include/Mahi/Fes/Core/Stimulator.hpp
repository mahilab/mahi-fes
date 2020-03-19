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
#include <Mahi/Fes/Core/Scheduler.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>
#include <mutex>
#include <string>
#include <vector>

namespace mahi {
namespace fes {
class Stimulator {
public:
    Stimulator(const std::string& name_, const std::string& com_port_,
               std::vector<Channel>& channels_, size_t size_);

    ~Stimulator();

    bool enable();

    void disable();

    std::string get_name();

    bool create_scheduler(const unsigned char sync_msg, double frequency_);

    bool is_enabled();

    void write_amp(Channel channel_, unsigned int amplitude_);

    void write_amps(std::vector<Channel> channels_, std::vector<int> amplitudes_);

    void write_pws(std::vector<Channel> channels_, std::vector<int> pulsewidths_);

    void write_pw(Channel channel_, unsigned int pw_);

    void update_max_amp(Channel channel_, unsigned int max_amp_);

    void update_max_pw(Channel channel_, unsigned int max_pw_);

    bool add_event(Channel channel_, unsigned char event_type = STIM_EVENT);

    bool add_events(std::vector<Channel> channels_, unsigned char event_type = STIM_EVENT);

    std::vector<Channel> get_channels();

    bool begin();

    bool update();

    bool halt_scheduler();

    size_t                   num_events;
    std::vector<int>         amplitudes;
    std::vector<int>         pulsewidths;
    std::vector<int>         max_amplitudes;
    std::vector<int>         max_pulsewidths;
    std::vector<std::string> channel_names;
    std::mutex               mtx;

private:
    // variables for serial communication
    HANDLE hComm;
    DCB    dcbSerialParams = {0};

    mahi::util::Time delay_time = mahi::util::milliseconds(1);

    std::string          name;
    std::string          com_port;
    bool                 enabled;
    std::vector<Channel> channels;
    Scheduler            scheduler;
    mahi::util::Time     setup_time = mahi::util::milliseconds(1);

    bool open_port();

    bool configure_port();

    bool initialize_board();

    void close_stimulator();
};
}  // namespace fes
}  // namespace mahi