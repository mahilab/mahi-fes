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

#include <Mahi/Fes/Core/Message.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>

namespace mahi {
namespace fes {

#define CRC_SEED 0xFFFF  // seed value for doing the crc calculation
#define CRC_POLY 0xA001  // poly value for doing the crc calculation

class ReadMessage : public Message {
public:
    /// ReadMessage constructor
    ReadMessage(std::vector<unsigned char> message);
    /// ReadMessage constructor with message count input
    ReadMessage(std::vector<unsigned char> message, size_t msg_count);
    /// calculate the cyclic redundancy check for the given message
    std::vector<unsigned char> calc_crc();
    /// returns the message data (without header or crc)
    std::vector<unsigned char> get_data();
    /// returns the type of the message as an unsigned char
    unsigned char get_read_message_type();
    /// checks if the read message is valid according to size, type, and crc
    bool is_valid();

    size_t                     m_msg_count = 0;      // the total number of messages received from the board
    unsigned char              m_read_message_type;  // the type of message-refer to utility.hpp for msg types
    std::vector<unsigned char> m_crc;                // the unsigned char values of the crc

    std::vector<unsigned char> valid_msg_types = {ERROR_REPORT_MSG, EVENT_ERROR_MSG, CREATE_SCHEDULE_REPLY_MSG,
                                                  CREATE_EVENT_REPLY_MSG,
                                                  EVENT_COMMAND_REPLY_MSG};  // list of message types that can be sent

private:
    std::vector<unsigned char> m_data;  // message data (without header or crc)
};

}  // namespace fes
}  // namespace mahi