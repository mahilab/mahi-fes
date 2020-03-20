#pragma once

#include <Mahi/Fes/Core/Message.hpp>
#include <Mahi/Fes/Utility/Utility.hpp>

namespace mahi {
namespace fes {

class ReadMessage : public Message {
public:
    ReadMessage(std::vector<unsigned char> message);
    ReadMessage(std::vector<unsigned char> message, size_t);

    size_t                     m_msg_count = 0;
    unsigned char              m_read_message_type;

    std::vector<unsigned char> valid_msg_types = {ERROR_REPORT_MSG, EVENT_ERROR_MSG,
                                                  CREATE_SCHEDULE_REPLY_MSG, CREATE_EVENT_REPLY_MSG,
                                                  EVENT_COMMAND_REPLY_MSG};

    std::vector<unsigned char> get_data();

private:
    std::vector<unsigned char> m_data;
    unsigned char get_read_message_type();
    bool          is_valid();
};

}  // namespace fes
}  // namespace mahi