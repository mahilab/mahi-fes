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

#include <Mahi/Fes/Core/Scheduler.hpp>
#include <Mahi/Fes/Core/WriteMessage.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::util;

namespace mahi {
namespace fes {

Scheduler::Scheduler() : m_id(0x01) {}

Scheduler::~Scheduler() { disable(); }

bool Scheduler::create_scheduler(HANDLE& hComm_, const unsigned char sync_char_, unsigned int duration,
                                 Time setup_time) {
    m_sync_char = sync_char_;

    m_hComm = hComm_;

    DWORD dwBytesWritten = 0;  // Captures how many bits were written

    // convert the input duration (int) into two bytes that we can send over a message
    std::vector<unsigned char> duration_chars = int_to_twobytes(duration);

    std::vector<unsigned char> crt_sched = {DEST_ADR,             // Destination
                                            SRC_ADR,              // Source
                                            CREATE_SCHEDULE_MSG,  // Msg type
                                            CREATE_SCHED_LEN,     // Message length
                                            m_sync_char,          // sync character
                                            duration_chars[0],    // schedule duration (byte 1)
                                            duration_chars[1],    // schedule duration (byte 2)
                                            0x00};                // checksum placeholder

    WriteMessage crt_sched_message(crt_sched);

    if (crt_sched_message.write(m_hComm, "Creating Scheduler")) {
        m_enabled = true;
        sleep(setup_time);
        return true;
    } else {
        return false;
    }
}

bool Scheduler::halt_scheduler() {
    if (is_enabled()) {
        std::vector<unsigned char> halt = {DEST_ADR,  // Destination
                                           SRC_ADR,   // Source
                                           HALT_MSG,  // Msg type
                                           HALT_LEN,  // Msg len
                                           m_id,      // Schedule ID
                                           0x00};     // Checksum placeholder

        WriteMessage halt_message(halt);

        return halt_message.write(m_hComm, "Schedule Closing");
    } else {
        LOG(Error) << "Scheduler was not enabled. Nothing to disable";
        return false;
    }
}

bool Scheduler::add_event(Channel channel_, Time sleep_time, bool is_virtual_, unsigned char event_type) {
    unsigned int num_events = (unsigned int)m_events.size();

    if (m_enabled) {
        for (unsigned int i = 0; i < num_events; i++) {
            if (m_events[i].get_channel_num() == channel_.get_channel_num()) {
                LOG(Error) << "Did not add event because an event already existed with that channel.";
                return false;
            }
        }

        // add 5 us delay so that they don't all occur at the exact same time
        auto delay_time = 5 * num_events;  // ms

        // add event to list of events
        m_events.push_back(Event(m_hComm, m_id, delay_time, channel_, (unsigned char)(num_events + 1),is_virtual_));

        sleep(sleep_time);

        return true;
    } else {
        LOG(Error) << "Scheduler is not yet enabled. ";
        return false;
    }
}

bool Scheduler::send_sync_msg() {
    if (m_enabled) {
        std::vector<unsigned char> sync = {DEST_ADR,      // Destination
                                           SRC_ADR,       // Source
                                           SYNC_MSG,      // Msg type
                                           SYNC_MSG_LEN,  // Message length
                                           m_sync_char,   // sync character
                                           0x00};         // Checksum placeholder

        WriteMessage sync_message(sync);

        if (sync_message.write(m_hComm, "Sending Sync Message")) {
            return true;
        } else {
            disable();
            return false;
        }
    } else {
        LOG(Error) << "Scheduler is not yet enabled";
        return false;
    }
}

void Scheduler::disable() {
    halt_scheduler();

    m_enabled = false;

    for (auto event = m_events.begin(); event != m_events.end(); event++) {
        event->delete_event();
    }

    std::vector<unsigned char> del_sched = {DEST_ADR,             // Destination
                                            SRC_ADR,              // Source
                                            DELETE_SCHEDULE_MSG,  // Msg type
                                            DEL_SCHED_LEN,        // Message length
                                            m_id,                 // Schedule ID
                                            0x00};                // Checksum placeholder

    WriteMessage del_sched_message(del_sched);

    del_sched_message.write(m_hComm, "Closing Schedule");
}

void Scheduler::set_amp(Channel channel_, unsigned int amplitude_) {
    // loop over available events in the scheduler
    for (auto event = m_events.begin(); event != m_events.end(); event++) {
        // if the event is for the correct channel we are looking for, write the amplitude and exit
        // the function
        if (event->get_channel_num() == channel_.get_channel_num()) {
            event->set_amplitude(amplitude_);
            return;
        }
    }
    // if we didnt find the event, something is messed up, so return false
    LOG(Error) << "Did not find the correct event to update on channel " << channel_.get_channel_name() <<  ". Nothing has changed.";
}

unsigned int Scheduler::get_amp(Channel channel_) {
    // loop over available events in the scheduler
    for (auto event = m_events.begin(); event != m_events.end(); event++) {
        // if the event is for the correct channel we are looking for, wrevente the ampleventude and
        // exevent the function
        if (event->get_channel_num() == channel_.get_channel_num()) {
            return event->get_amplitude();
        }
    }
    // if we didnt find the event, something is messed up, so return 0
    LOG(Error) << "Did not find the correct event to pull from on channel " << channel_.get_channel_name() << ". Returning 0.";
    return 0;
}

void Scheduler::write_pw(Channel channel_, unsigned int pw_) {
    // loop over available events in the scheduler
    for (auto event = m_events.begin(); event != m_events.end(); event++) {
        // if the event is for the correct channel we are looking for, wrevente the ampleventude and
        // exevent the function
        if (event->get_channel_num() == channel_.get_channel_num()) {
            event->set_pulsewidth(pw_);
            return;
        }
    }
    // if we didnt find the event, something is messed up, so return false
    LOG(Error) << "Did not find the correct event to update on channel " << channel_.get_channel_name() <<  ". Nothing has changed.";
}

unsigned int Scheduler::get_pw(Channel channel_) {
    // loop over available events in the scheduler
    for (auto event = m_events.begin(); event != m_events.end(); event++) {
        // if the event is for the correct channel we are looking for, wrevente the ampleventude and
        // exevent the function
        if (event->get_channel_num() == channel_.get_channel_num()) {
            return event->get_pulsewidth();
        }
    }
    // if we didnt find the event, something is messed up, so return 0
    LOG(Error) << "Did not find the correct event to pull from on channel " << channel_.get_channel_name() << ". Returning 0.";
    return 0;
}

bool Scheduler::update() {
    // loop over available events in the scheduler
    for (auto event = m_events.begin(); event != m_events.end(); event++) {
        // If any channel fails to update, return false after throwing an error
        if (!event->update()) {
            LOG(Error) << "Channel " << event->get_channel_name() << " failed to update";
            return false;
        }
    }
    return true;
}

size_t Scheduler::get_num_events() { return m_events.size(); }

std::vector<Event> Scheduler::get_events() { return m_events; }

unsigned char Scheduler::get_id() { return m_id; }

void Scheduler::set_id(unsigned char sched_id_) { m_id = sched_id_; }

bool Scheduler::is_enabled() { return m_enabled; }
}  // namespace fes
}  // namespace mahi
