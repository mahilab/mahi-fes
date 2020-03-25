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
#define DELETE_EVENT_LEN        0x01
#define CHANGE_EVENT_PARAMS_LEN 0x04
#define STIM_EVENT              0x03

public:
    /// Event constructor
    Event(HANDLE& hComm, unsigned char schedule_id_, int delay_time_, Channel channel_, unsigned char event_id_,
          unsigned int pulse_width_ = 0, unsigned int amplitude_ = 0, unsigned char event_type_ = STIM_EVENT,
          unsigned char priority_ = 0x00, unsigned char zone_ = 0x00);
    /// Event destructor
    ~Event();
    /// Sends the message to the UECU to create a new event given the constructor params
    bool create_event();
    /// Sends the message to the UECU to delete the event
    bool delete_event();
    /// Sends the edit event message given the current amplitude and pulsewidth values
    bool update();
    /// returns the current amplitude
    unsigned int get_amplitude();
    /// returns the current pulsewidth
    unsigned int get_pulsewidth();
    /// returns the channel number of the channel attached to this event
    unsigned char get_channel_num();
    /// returns the name of the channel attached to this event
    std::string get_channel_name();
    /// sets the amplitude of the event (does not write to UECU)
    void set_amplitude(unsigned int amplitude_);
    /// sets the pulsewidth of the event (does not write to UECU)
    void set_pulsewidth(unsigned int pulse_width_);
    /// sets the event id as received in a message from the UECU after setting up
    void set_event_id(unsigned char event_id);

private:
    HANDLE        m_hComm;            // serial handle to the appropriate UECU
    unsigned char m_schedule_id;      // schedule id of the associated schedule
    unsigned int  m_delay_time;       // delay time from the beginning of the schedule (all events should be different)
    Channel       m_channel;          // channel attached to the event
    unsigned int  m_pulse_width;      // current pulse-width value
    unsigned int  m_amplitude;        // current amplitude value
    unsigned char m_event_type;       // event type associated to the event
    unsigned char m_priority;         // priority level for events (less has more priority)
    unsigned char m_event_id;         // event id returned from the UECU at event creation time
    unsigned int  m_max_amplitude;    // max amplitude allowed for the evenet
    unsigned int  m_max_pulse_width;  // max pulse width allowed for the event
    unsigned char m_zone;             // unused (should be 0x00)
};
}  // namespace fes
}  // namespace mahi
