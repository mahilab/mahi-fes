#include "FES/Core/ReadMessage.hpp"

namespace fes{

ReadMessage::ReadMessage(std::vector<unsigned char> message)
    {
        m_message = message;
        m_size = m_message.size();
        m_checksum = m_message.back();
    }
} // namespace fes