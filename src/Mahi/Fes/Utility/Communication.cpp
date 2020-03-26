// #include <Mahi/Fes/Core/ReadMessage.hpp>
#include <Mahi/Fes/Utility/Communication.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::util;

namespace mahi {
namespace fes {
std::vector<ReadMessage> get_all_messages(HANDLE hComm) {
    std::vector<ReadMessage> incoming_messages;
    // while there are still messages, continue to read them
    while (true) {
        std::vector<unsigned char> inc_message = read_message(hComm, false);
        // if there was no recent message, exit.
        if (inc_message.empty()){
            break;
        }
        else{
            incoming_messages.push_back(ReadMessage(inc_message));
        }
    }
    return incoming_messages;
}

void process_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages) {
    for (size_t i = 0; i < inc_messages.size(); i++) {
        ReadMessage current_message = inc_messages.front();
        inc_messages.pop();
        print_message(current_message.get_message());
    }
}

std::vector<unsigned char> read_message(HANDLE hComm, bool should_wait, Time timeout) {
    DWORD         header_size = 8;
    unsigned char msg_header[8];
    DWORD         dwBytesRead = 0;

    bool  message_received = false;
    Clock timeout_clock;

    std::vector<unsigned char> msg;

    while (!message_received && ((timeout_clock.get_elapsed_time() < timeout) && should_wait)) {
        if (!ReadFile(hComm, msg_header, header_size, &dwBytesRead, NULL)) {
            LOG(Error) << "Could not read message header. Returning empty vector.";
        } else if (dwBytesRead != 0) {
            DWORD body_size = (unsigned int)msg_header[7] + 2;

            std::unique_ptr<unsigned char[]> msg_body(new unsigned char[body_size]);
            if (!ReadFile(hComm, msg_body.get(), body_size, &dwBytesRead, NULL)) {
                LOG(Error) << "Could not read message body. Returning empty vector.";
            } else {
                if (msg_header[4] == (unsigned char)0x80 && msg_header[5] == (unsigned char)0x04) {
                    for (unsigned int i = 0; i < (header_size + body_size); i++) {
                        if (i < header_size) {
                            msg.push_back(msg_header[i]);
                        } else {
                            msg.push_back(msg_body[i - header_size]);
                        }
                    }
                    message_received = true;
                } else {
                    LOG(Error) << "Invalid Message Header Received: ";
                    std::vector<unsigned char> msg_header_vec(std::begin(msg_header), std::end(msg_header));
                    print_message(msg_header_vec);
                    return msg_header_vec;
                }
            }
        }
    }
    if (!message_received && should_wait) {
        LOG(Error) << "Ran into timeout when waiting to receive a message. Returning empty message instead.";
    }
    // print_message(msg);
    return msg;
}

}  // namespace fes
}  // namespace mahi