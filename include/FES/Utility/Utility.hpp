#pragma once

#include "Windows.h"

namespace fes{
    int checksum(unsigned char myarray[], int array_size);

    std::vector<unsigned char> int_to_twobytes(int input_int);

    bool write_message(HANDLE hComm_, unsigned char* message_, const int message_size_, const std::string& activity);
}
