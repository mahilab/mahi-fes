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

# pragma once

#include <Mahi/Fes/Core/Message.hpp>
#include <string>

#include "Windows.h"

namespace mahi {
namespace fes {

/// Structure of byte arrays:
///  Destination address - always 0x04
///  Source address - always 0x80
///  Message type - see message commands in Utility.hpp
///  Message Length - length of message without header (everything up to msg length) or checksum
///  Message - Bytes of the message - must be of length Message Length
///  Checksum Calculation - add each of the bytes, mask the lower byte of sum and add cary byte,
///  then invert sum
class WriteMessage : public Message {
public:
    /// WriteMessage constructor
    WriteMessage(std::vector<unsigned char> message);
    /// calculates the checksum according to the UECU instructions
    unsigned char calc_checksum();
    /// returns the member variable m_checksum which has already been created
    unsigned char get_checksum();
    /// writes the message to the serial port
    bool write(HANDLE hComm, const std::string& activity);
    
    unsigned char m_checksum;  // checksum of the given message

private:
    /// adds the checksum (unsigned char) to the last index of the message
    void add_checksum();
};

}  // namespace fes
}  // namespace mahi