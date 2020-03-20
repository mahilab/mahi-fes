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

#include <Mahi/Fes/Core/Channel.hpp>
#include <Mahi/Fes/Core/Event.hpp>
#include <Mahi/Util.hpp>
#include <vector>

namespace mahi {
namespace fes {
class Scheduler {
private:
    unsigned char              id;
    std::vector<unsigned char> event_ids;
    std::vector<Event>         events;
    bool                       enabled;
    HANDLE                     hComm;
    unsigned char              sync_char;

#define DEL_SCHED_LEN 0x01
#define STIM_EVENT    0x03

public:
    Scheduler();
    ~Scheduler();

    bool create_scheduler(HANDLE& hComm_, const unsigned char sync_msg, unsigned int duration,
                          mahi::util::Time setup_time);

    bool add_event(Channel channel_, mahi::util::Time sleep_time, unsigned char event_type = STIM_EVENT);

    bool edit_event(Channel channel_);

    void enable();

    void disable();

    unsigned char get_id();
    
    void set_id(unsigned char sched_id_);

    void write_amp(Channel channel_, unsigned int amplitude_);

    unsigned int get_amp(Channel channel_);

    void write_pw(Channel channel_, unsigned int pw_);

    unsigned int get_pw(Channel channel_);

    size_t get_num_events();

    bool halt_scheduler();

    bool update();

    bool send_sync_msg();

    bool is_enabled();
};
}  // namespace fes
}  // namespace mahi