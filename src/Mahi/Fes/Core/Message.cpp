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

#include <Mahi/Fes/Core/Message.hpp>

namespace mahi {
namespace fes {

Message::Message() {}

size_t Message::get_size() { return m_size; }

unsigned char Message::get_checksum() { return m_checksum; }

std::vector<unsigned char> Message::get_message() { return m_message; }

unsigned char* Message::get_message_pointer() { return &m_message[0]; }

unsigned char Message::calc_checksum() {
    int csum = 0;
    for (int i = 0; i < m_size - 1; i++) {
        csum += m_message[i];
    }
    csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
    return csum;
}

Message::~Message() {}

}  // namespace fes
}  // namespace mahi