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

std::vector<unsigned char> Message::get_message() { return m_message; }

unsigned char* Message::get_message_pointer() { return &m_message[0]; }

Message::~Message() {}

}  // namespace fes
}  // namespace mahi