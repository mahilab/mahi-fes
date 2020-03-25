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

#include <Mahi/Util.hpp>
#include <vector>

#include "Windows.h"

using namespace mahi::util;

namespace mahi {
namespace fes {

std::string print_as_hex(unsigned char num){
    char char_buff[20];
    sprintf(char_buff, "0x%02X", (unsigned int)num);
    return std::string(char_buff);
}

std::vector<unsigned char> int_to_twobytes(int input_int) {
    // value to do comparison
    int byte_size = 256;

    // initialize empty vector of unsigned char
    std::vector<unsigned char> char_vec_out;

    // integer division for the first byte (eg. 0 if < 256, 1 if >=256 && < 512, 2 if >=512 && <
    // 768, etc)
    char_vec_out.push_back(input_int / byte_size);

    // remainder after dividing by 256 (will always be between 1 and 256)
    char_vec_out.push_back(input_int % byte_size);

    return char_vec_out;
}

void print_message(std::vector<unsigned char> message) {
    std::cout << "Message: ";
    for (size_t i = 0; i < message.size(); i++) {
        std::cout << print_as_hex(message[i]);
        if (i != message.size()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}
}  // namespace fes
}  // namespace mahi