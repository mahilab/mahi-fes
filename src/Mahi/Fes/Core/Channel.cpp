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

#include <Windows.h>
#include <tchar.h>

#include <Mahi/Fes/Core/Channel.hpp>
#include <Mahi/Fes/Core/WriteMessage.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::util;

namespace mahi {
namespace fes {

Channel::Channel(const std::string& name_, unsigned char channel_num_, unsigned char an_ca_nums_,
                 unsigned int max_amp_, unsigned int max_pw_, unsigned int ip_delay_,
                 unsigned char aspect_) :
    name(name_),
    channel_num(channel_num_),
    an_ca_nums(an_ca_nums_),
    max_amp(max_amp_),
    max_pw(max_pw_),
    ip_delay(ip_delay_),
    aspect(aspect_){

    };

Channel::~Channel() {}

bool Channel::setup_channel(HANDLE serial_handle_, Time delay_time_) {
    std::vector<unsigned char> ip_delay_bytes = int_to_twobytes(ip_delay);

    std::vector<unsigned char> setup = {DEST_ADR,                // Destination
                                        SRC_ADR,                 // Source
                                        CHANNEL_SETUP_MSG,       // Msg type
                                        CH_SET_LEN,              // Msg len
                                        channel_num,             // Channel
                                        (unsigned char)max_amp,  // AmpLim
                                        (unsigned char)max_pw,   // PWLim
                                        ip_delay_bytes[0],       // IP delay (byte 1)
                                        ip_delay_bytes[1],       // IP delay (byte 2)
                                        ONE_TO_ONE,              // Aspect
                                        an_ca_nums,              // Anode Cathode
                                        0x00};                   // Checksum

    WriteMessage setup_message(setup);

    if (setup_message.write(serial_handle_, "Setting Up Channel")) {
        // Sleep for delay time to allow the board to process
        sleep(delay_time_);
        return true;
    } else {
        return false;
    }
}

unsigned int Channel::get_max_amplitude() { return max_amp; }

unsigned int Channel::get_max_pulse_width() { return max_pw; }

void Channel::set_max_amplitude(unsigned int max_amp_) { max_amp = max_amp_; }

void Channel::set_max_pulse_width(unsigned int max_pw_) { max_pw = max_pw_; }

unsigned char Channel::get_channel_num() { return channel_num; }

std::string Channel::get_channel_name() { return name; }
}  // namespace fes
}  // namespace mahi
