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

namespace mahi {
namespace fes {
class Event {
private:
#define DELETE_EVENT_LEN        0x01
#define CHANGE_EVENT_PARAMS_LEN 0x04
#define STIM_EVENT              0x03

    HANDLE        hComm;
    unsigned char schedule_id;
    int           delay_time;
    Channel       channel;
    unsigned int  pulse_width;
    unsigned int  amplitude;
    unsigned char event_type;
    unsigned char priority;
    unsigned char zone;
    unsigned char event_id;
    unsigned int  max_amplitude;
    unsigned int  max_pulse_width;

public:
    // Event();

    Event(HANDLE& hComm, unsigned char schedule_id_, int delay_time_, Channel channel_,
          unsigned char event_id_, unsigned int pulse_width_ = 0, unsigned int amplitude_ = 0,
          unsigned char event_type_ = STIM_EVENT, unsigned char priority_ = 0x00,
          unsigned char zone_ = 0x00);

    ~Event();
    bool create_event();

    unsigned char get_channel_num();

    std::string get_channel_name();

    bool edit_event();

    bool delete_event();

    bool update();

    void set_amplitude(unsigned int amplitude_);

    void set_pulsewidth(unsigned int pulse_width_);

    void set_event_id(unsigned char event_id);

    unsigned int get_amplitude();

    unsigned int get_pulsewidth();
};
}  // namespace fes
}  // namespace mahi
