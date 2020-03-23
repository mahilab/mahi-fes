# pragma once

#include <Mahi/Fes/Core/Message.hpp>
#include <string>

#include "Windows.h"

namespace mahi {
namespace fes {

class WriteMessage : public Message {
public:
    /// constructor for the write message class
    WriteMessage(std::vector<unsigned char> message);

    /// calculates the checksum according to the UECU instructions
    unsigned char calc_checksum();

    /// returns the member variable m_checksum which has already been created
    unsigned char get_checksum();

    /// writes the message to the serial port
    bool write(HANDLE hComm, const std::string& activity);

    unsigned char m_checksum;  ///< checksum of the given message

private:
    /// adds the checksum (unsigned char) to the last index of the message
    void add_checksum();
};

}  // namespace fes
}  // namespace mahi