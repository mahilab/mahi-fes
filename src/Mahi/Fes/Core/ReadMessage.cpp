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

#include <Mahi/Fes/Core/ReadMessage.hpp>
#include <Mahi/Util.hpp>
#include <algorithm>

using namespace mahi::util;

namespace mahi {
namespace fes {

ReadMessage::ReadMessage(std::vector<unsigned char> message) {
    m_message           = message;
    m_size              = message.size();
    m_checksum          = m_size > 0 ? m_message.back() : 0x00;
    m_read_message_type = m_size > 1 ? m_message[6] : 0x00;
    std::vector<unsigned char> data_vec;
    if(m_size > 2) std::copy(m_message.begin() + 8, m_message.begin() + m_size - 2, std::back_inserter(data_vec));
    m_data = data_vec;
}

ReadMessage::ReadMessage(std::vector<unsigned char> message, size_t msg_count) {
    m_message           = message;
    m_size              = m_message.size();
    m_checksum          = m_size > 0 ? m_message.back() : 0x00;
    m_read_message_type = m_size > 1 ? m_message[6] : 0x00;
    m_msg_count         = msg_count;
    std::vector<unsigned char> data_vec;
    if(m_size > 2) std::copy(m_message.begin() + 8, m_message.begin() + m_size - 2, std::back_inserter(data_vec));
    m_data = data_vec;
}

unsigned char ReadMessage::get_read_message_type() { return m_read_message_type; }

bool ReadMessage::is_valid() {
    if (calc_checksum() != m_checksum) {
        LOG(Error) << "Read checksum is wrong; message is likely invalid.";
        return false;
    } else if (!binary_search(valid_msg_types.begin(), valid_msg_types.end(),
                              m_read_message_type)) {
        LOG(Error) << "Message type " << m_read_message_type
                   << " is unknown. Cannot interpret message.";
        return false;
    }
    return true;
}

std::vector<unsigned char> ReadMessage::get_data(){
    return m_data;
}

}  // namespace fes
}  // namespace mahi