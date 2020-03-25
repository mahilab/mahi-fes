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

#include <string>
#include <vector>

#include "Windows.h"

namespace mahi {
namespace fes {
/// converts a single integer to a vector of correct unsigned chars
std::vector<unsigned char> int_to_twobytes(int input_int);
/// converst a single unsigned char to a hex string (0xAB)
std::string print_as_hex(unsigned char num);
/// prints a single message that was given as an argument
void print_message(std::vector<unsigned char> message);

// Message commands for sending serial packets
#define TRIGGER_SETUP_MSG         0x03
#define HALT_MSG                  0x04
#define ERROR_REPORT_MSG          0x05
#define EVENT_ERROR_MSG           0x06
#define CREATE_SCHEDULE_MSG       0x10
#define CREATE_SCHEDULE_REPLY_MSG 0x11
#define DELETE_SCHEDULE_MSG       0x12
#define CHANGE_SCHEDULE_MSG       0x13
#define CHANGE_SCHEDULE_STATE_MSG 0x14
#define CREATE_EVENT_MSG          0x15
#define CREATE_EVENT_REPLY_MSG    0x16
#define DELETE_EVENT_MSG          0x17
#define CHANGE_EVENT_SCHED_MSG    0x18
#define CHANGE_EVENT_PARAMS_MSG   0x19
#define SYNC_MSG                  0x1B
#define EVENT_COMMAND_MSG         0x1C
#define CHANNEL_SETUP_MSG         0x47
#define EVENT_COMMAND_REPLY_MSG   0x49

// Predefined destination and sources addresses to be used for all communication
#define DEST_ADR 0x04
#define SRC_ADR  0x80

// Length of messages which should never be changed
#define SYNC_MSG_LEN     0x01
#define CREATE_SCHED_LEN 0x03
#define CH_SET_LEN       0x07
#define CR_EVT_LEN       0x09
#define HALT_LEN         0x01

// Anode Cathode pairs for channels 1-4
#define AN_CA_1 0x01
#define AN_CA_2 0x23
#define AN_CA_3 0x45
#define AN_CA_4 0x67

// Event type
#define STIM_EVENT 0x03
}  // namespace fes
}  // namespace mahi
