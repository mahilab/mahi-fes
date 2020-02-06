#include "Windows.h"
#include <MEL/Logging/Log.hpp>
#include <vector>

using namespace mel;

namespace fes{
    int checksum(unsigned char myarray[], int array_size){  // checksum is a function that preforms checksums of all of the byte strings used in this code
        int csum = 0;
        for (int i = 0; i < array_size - 1; i++) {
            csum += myarray[i];
        }
        csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
        return csum;
    }

    std::vector<unsigned char> int_to_twobytes(int input_int){
        int byte_size = 256;
        std::vector<unsigned char> char_vec_out;
        char_vec_out.push_back(input_int/byte_size);
        char_vec_out.push_back(input_int%byte_size);
        return char_vec_out;
    }

    bool write_message(HANDLE hComm_, unsigned char message_[], const std::string& activity){
        
        // Captures how many bits were written
        DWORD dwBytesWritten = 0; 
        
        // add the checksum to the last message 
        message_[(sizeof(message_) / sizeof(*message_)) - 1] = checksum(message_, (sizeof(message_) / sizeof(*message_)));
        
        // write the file iff possible
        if(!WriteFile(hComm_, message_, (sizeof(message_) / sizeof(*message_)),&dwBytesWritten,NULL)){
            LOG(Error) << "Error Closing Schedule.";
            return false;
        }
        else{
            LOG(Info) << "Schedule Closing was Successful.";
            return true;
        }
    }
}
