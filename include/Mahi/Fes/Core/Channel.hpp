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

#include <Mahi/Util.hpp>
#include <string>

namespace mahi {
namespace fes {

// definition of channels 1-8
#define CH_1 0x00
#define CH_2 0x01
#define CH_3 0x02
#define CH_4 0x03
#define CH_5 0x04
#define CH_6 0x05
#define CH_7 0x06
#define CH_8 0x07

// definition of aspect ratios
#define ONE_TO_ONE 0x11
// #define TWO_TO_ONE 0x21

class Channel {
private:
    std::string   name;
    unsigned char aspect;
    unsigned char channel_num;
    unsigned char an_ca_nums;
    unsigned int  max_amp;   // maximum amplitude
    unsigned int  max_pw;    // maximum pulsewidth
    unsigned int  ip_delay;  // interphase delay
    unsigned char event_id;

public:
    Channel(const std::string& name_, unsigned char channel_num_, unsigned char an_ca_nums_,
            unsigned int max_amp_, unsigned int max_pw_, unsigned int ip_delay_ = 100,
            unsigned char aspect_ = ONE_TO_ONE);

    ~Channel();

    bool setup_channel(HANDLE serial_handle_, mahi::util::Time delay_time_);

    unsigned int get_max_amplitude();

    unsigned int get_max_pulse_width();

    void set_max_amplitude(unsigned int);

    void set_max_pulse_width(unsigned int);

    unsigned char get_channel_num();

    std::string get_channel_name();
};
}  // namespace fes
}  // namespace mahi