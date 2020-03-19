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

    /// writes the message to the serial port
    bool write(HANDLE hComm, const std::string& activity);

private:
    /// adds the checksum (unsigned char) to the last index of the message
    void add_checksum();
};

}  // namespace fes
}  // namespace mahi