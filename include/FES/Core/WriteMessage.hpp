#include "FES/Core/Message.hpp"
#include "Windows.h"
#include <string>

namespace fes{
    
class WriteMessage : public Message
{
public:
    /// constructor for the write message class
    WriteMessage(std::vector<unsigned char> message);
    
    /// writes the message to the serial port
    bool write(HANDLE hComm, const std::string& activity);
private:
    /// adds the checksum (unsigned char) to the last index of the message
    void add_checksum();
};

} // namespace fes