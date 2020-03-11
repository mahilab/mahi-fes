#include "FES/Core/WriteMessage.hpp"
#include "Mahi/Util.hpp"

using namespace mahi::util;

namespace fes{

WriteMessage::WriteMessage(std::vector<unsigned char> message)
    {
        m_message = message;
        m_size = m_message.size();
        m_checksum = calc_checksum();
        m_message[m_size-1] = m_checksum;
    }

bool WriteMessage::write(HANDLE hComm, const std::string& activity){
    
    // dont log anything if the input string is "NONE"
    bool log_message = (activity.compare("NONE") != 0);
    
    // Captures how many bits were written
    DWORD dwBytesWritten = 0; 
    
    // write the file if possible
    if(!WriteFile(hComm, get_message_pointer(), (DWORD)m_size, &dwBytesWritten,NULL)){
        // log that the activity was successful or unsuccessful
        if (log_message) {
            LOG(Error) << "Error " << activity;
        }
        return false;
    }
    else{
        if (log_message) {
            LOG(Info) << activity << " was Successful.";
        }
        return true;
    }
}

} // namespace fes