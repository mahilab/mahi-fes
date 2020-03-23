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

#include <vector>

namespace mahi {
namespace fes {

/// Generic class for capturing a message
class Message {
public:
    Message();
    ~Message();
    size_t                     get_size();
    std::vector<unsigned char> get_message();

protected:
    std::vector<unsigned char> m_message;  ///< vector of the message itself

    size_t m_size;  ///< size of the given message

    unsigned char m_message_type;  ///< type of message

    /// Returns a pointer to the first element of the message vector
    unsigned char* get_message_pointer();
};

}  // namespace fes
}  // namespace mahi