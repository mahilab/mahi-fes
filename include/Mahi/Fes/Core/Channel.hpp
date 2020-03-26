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
#define TWO_TO_ONE 0x21

namespace mahi {
namespace fes {

/// The stimulator has 8 channels, and these channels are passed into the stimulator to see
/// what is available. This means that channels are created first, and controls much of the
/// behaviour of the stimulator by defining max values for each channel, interphase delays,
/// and aspect ratios. Any command applied to the channels will be based on these values.
class Channel {
public:
    /// Channel constructor
    Channel(const std::string& name_, unsigned char channel_num_, unsigned char an_ca_nums_,
            unsigned int max_amp_, unsigned int max_pw_, unsigned int ip_delay_ = 100,
            unsigned char aspect_ = ONE_TO_ONE);
    /// Channel destructor
    ~Channel();
    /// writes the channel setup command to the UECU given the constructor parameters.
    bool setup_channel(HANDLE serial_handle_, mahi::util::Time delay_time_);
    /// return the max amplitude allowed by the channel
    unsigned int get_max_amplitude();
    /// return the max pulsewidth allowed by the channel
    unsigned int get_max_pulse_width();
    /// return the channel number of the channel
    unsigned char get_channel_num();
    /// return the name of the channel
    std::string get_channel_name();
    /// set the maximum allowed amplitude for the channel
    void set_max_amplitude(unsigned int);
    /// set the maximum allowed pulsewidth for the channel
    void set_max_pulse_width(unsigned int);

private:
    std::string   m_name;         // name of the channel-this is used for changing channel parameters
    unsigned char m_aspect;       // the aspect ratio for the channel: (first 4 bits)/(second 4 bits)
    unsigned char m_channel_num;  // channel number as #defined in this file
    unsigned char m_an_ca_nums;   // anode cathode channel numbers
    unsigned int  m_max_amp;      // maximum amplitude
    unsigned int  m_max_pw;       // maximum pulsewidth
    unsigned int  m_ip_delay;     // interphase delay
};
}  // namespace fes
}  // namespace mahi
