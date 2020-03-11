#include "FES/Core/Message.hpp"
// #include "FES/Utility/Utility.hpp"

namespace fes{

Message::Message(){
}

size_t Message::get_size(){
    return m_size;
}

unsigned char Message::get_checksum(){
    return m_checksum;
}

std::vector<unsigned char> Message::get_message(){
    return m_message;
}

unsigned char* Message::get_message_pointer(){
    return &m_message[0];
}

unsigned char Message::calc_checksum(){  
    int csum = 0;
    for (int i = 0; i < m_size - 1; i++) {
        csum += m_message[i];
    }
    csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
    return csum;
}

Message::~Message(){
}


} // namespace fes