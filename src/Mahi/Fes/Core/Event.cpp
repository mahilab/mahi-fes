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

#include <Mahi/Fes/Core/Channel.hpp>
#include <Mahi/Fes/Core/Event.hpp>
#include <Mahi/Fes/Core/Stimulator.hpp>
#include <Mahi/Fes/Core/WriteMessage.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>
#include <Mahi/Fes/Utility/Communication.hpp>
#include <Mahi/Util.hpp>
#include <queue>

using namespace mahi::util;

namespace mahi {
namespace fes {

Event::Event(HANDLE& hComm_, unsigned char schedule_id_, int delay_time_, Channel channel_,
             unsigned char event_id_, unsigned int pulse_width_, unsigned int amplitude_,
             unsigned char event_type_, unsigned char priority_, unsigned char zone_) :
    hComm(hComm_),
    schedule_id(schedule_id_),
    delay_time(delay_time_),
    channel(channel_),
    pulse_width(pulse_width_),
    amplitude(amplitude_),
    event_type(event_type_),
    priority(priority_),
    event_id(event_id_),
    max_amplitude(channel.get_max_amplitude()),
    max_pulse_width(channel.get_max_pulse_width()),
    zone(zone_) {
    create_event();
}

Event::~Event() {}

bool Event::create_event() {
    std::vector<unsigned char> delay_time_chars = int_to_twobytes(delay_time);

    std::vector<unsigned char> create_event = {DEST_ADR,             // Destination
                                               SRC_ADR,              // Source
                                               CREATE_EVENT_MSG,     // Msg type
                                               CR_EVT_LEN,           // Message length
                                               schedule_id,          // Schedule ID
                                               delay_time_chars[0],  // Delay time (byte 1)
                                               delay_time_chars[1],  // Delay time (byte 2)
                                               priority,             // priority (default none)
                                               STIM_EVENT,           // Event type
                                               channel.get_channel_num(),   // Channel number
                                               (unsigned char)pulse_width,  // Pulse Width
                                               (unsigned char)amplitude,    // Amplitude
                                               zone,                        // Zone
                                               0x00};                       // Checksum Placeholder

    WriteMessage create_event_message(create_event);

    if (create_event_message.write(hComm, "Creating Event")) {
        sleep(milliseconds(100));
        std::queue<ReadMessage> inc_messages;
        ReadMessage event_created_msg = wait_for_message(hComm, inc_messages);
        for (size_t i = 0; i < event_created_msg.get_data().size(); i++) {
            std::cout << print_as_hex(event_created_msg.get_data()[i]) << ", ";
        }
        std::cout <<std::endl;
        set_event_id(event_created_msg.get_data()[0]);
        return true;
    } else {
        return false;
    }
}

void Event::set_amplitude(unsigned int amplitude_) {
    if (amplitude_ > max_amplitude) {
        amplitude = max_amplitude;
        LOG(Warning) << "Commanded too high of a amplitude on " << get_channel_name()
                     << " channel. It was clamped to " << max_amplitude << ".";
    } else if (amplitude_ < 0) {
        amplitude = 0;
        LOG(Warning) << "Commanded a negative amplitude on " << get_channel_name()
                     << " channel. It was clamped to 0.";
    } else {
        amplitude = amplitude_;
    }
}

unsigned int Event::get_amplitude() { return amplitude; }

void Event::set_pulsewidth(unsigned int pulsewidth_) {
    if (pulsewidth_ > max_pulse_width) {
        pulse_width = max_pulse_width;
        LOG(Warning) << "Commanded too high of a pulsewidth on " << get_channel_name()
                     << " channel. It was clamped to " << max_pulse_width << ".";
    } else if (pulsewidth_ < 0) {
        pulse_width = 0;
        LOG(Warning) << "Commanded a negative pulsewidth on " << get_channel_name()
                     << " channel. It was clamped to 0.";
    } else {
        pulse_width = pulsewidth_;
    }
}

unsigned int Event::get_pulsewidth() { return pulse_width; }

bool Event::update() {
    std::vector<unsigned char> edit_event = {DEST_ADR,                    // Destination
                                             SRC_ADR,                     // Source
                                             CHANGE_EVENT_PARAMS_MSG,     // Msg type
                                             CHANGE_EVENT_PARAMS_LEN,     // Msg len
                                             event_id,                    // Event ID
                                             (unsigned char)pulse_width,  // Pulsewidth to update
                                             (unsigned char)amplitude,    // Amplitude to update
                                             0x00,   // Placeholder for other parameters
                                             0x00};  // Checksum placeholder

    WriteMessage edit_event_message(edit_event);

    if (edit_event_message.write(hComm, "NONE")) {
        return true;
    } else {
        return false;
    }
}

void Event::set_event_id(unsigned char event_id_){
    event_id = event_id_;
}

bool Event::delete_event() {
    std::vector<unsigned char> del_evt = {DEST_ADR,          // Destination
                                          SRC_ADR,           // Source
                                          DELETE_EVENT_MSG,  // Msg type
                                          DELETE_EVENT_LEN,  // Msg len
                                          event_id,          // Event ID
                                          0x00};             // Checksum placeholder

    WriteMessage del_evt_message(del_evt);

    if (del_evt_message.write(hComm, "Deleting Event")) {
        return true;
    } else {
        return false;
    }
}

unsigned char Event::get_channel_num() { return channel.get_channel_num(); }

std::string Event::get_channel_name() { return channel.get_channel_name(); }
}  // namespace fes
}  // namespace mahi