// #include <Mahi/Fes/Core/ReadMessage.hpp>
#include <Mahi/Fes/Utility/Communication.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::util;

namespace mahi {
namespace fes {
void check_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages) {
    while (true) {
        size_t inc_msg_count = inc_messages.size();
        DWORD         header_size = 4;
        unsigned char msg_header[4];

        DWORD dwBytesRead = 0;

        if (!ReadFile(hComm, msg_header, header_size, &dwBytesRead, NULL)) {
            LOG(Error) << "Could not read message header";
        }
        if (dwBytesRead != 0) {
            DWORD body_size = (unsigned int)msg_header[3] + 1;

            std::unique_ptr<unsigned char[]> msg_body(new unsigned char[body_size]);
            if (!ReadFile(hComm, msg_body.get(), body_size, &dwBytesRead, NULL)) {
                LOG(Error) << "Could not read message body";
            } else {
                if (msg_header[0] == (unsigned char)0x04 && msg_header[1] == (unsigned char)0x80) {
                    inc_msg_count += 1;
                    std::vector<unsigned char> msg;
                    for (unsigned int i = 0; i < (header_size + body_size); i++) {
                        if (i < header_size)
                            msg.push_back(msg_header[i]);
                        else
                            msg.push_back(msg_body[i - header_size]);
                    }
                    ReadMessage received_msg(msg, inc_msg_count);
                    inc_messages.push(received_msg);
                } else {
                    std::cout << "invalid message: ";
                    for (size_t j = 0; j < header_size; j++) {
                        std::cout << (unsigned int)msg_header[j];
                        if (j != header_size)
                            std::cout << ", ";
                    }
                    std::cout << "\n";
                }
            }
        } else {
            std::cout << "no messages\n";
            break;
        }
    }
    process_inc_messages(hComm, inc_messages);
}

ReadMessage wait_for_message(HANDLE hComm, std::queue<ReadMessage> &inc_messages) {
    size_t inc_msg_count = inc_messages.size();
    DWORD         header_size = 8;
    unsigned char msg_header[8];

    DWORD dwBytesRead = 0;

    ReadMessage received_msg(std::vector<unsigned char>{}, inc_msg_count);

    bool message_received = false;
    Clock timeout_clock;

    while (!message_received && (timeout_clock.get_elapsed_time().as_seconds() < 1)) {
        if (!ReadFile(hComm, msg_header, header_size, &dwBytesRead, NULL)) {
            LOG(Error) << "Could not read message header";
        } else if (dwBytesRead != 0) {
            DWORD body_size = (unsigned int)msg_header[7] + 2;

            std::unique_ptr<unsigned char[]> msg_body(new unsigned char[body_size]);
            if (!ReadFile(hComm, msg_body.get(), body_size, &dwBytesRead, NULL)) {
                LOG(Error) << "Could not read message body";
            } else {
                if (msg_header[4] == (unsigned char)0x80 && msg_header[5] == (unsigned char)0x04) {
                    inc_msg_count += 1;
                    std::vector<unsigned char> msg;
                    for (unsigned int i = 0; i < (header_size + body_size); i++) {
                        if (i < header_size){
                            msg.push_back(msg_header[i]);
                        }
                        else{
                            msg.push_back(msg_body[i - header_size]);
                        }
                    }
                    
                    received_msg = ReadMessage(msg, inc_msg_count);
                    message_received = true;
                } else {
                    std::cout << "invalid message: ";
                    for (size_t j = 0; j < header_size; j++) {
                        std::cout << print_as_hex(msg_header[j]);
                        if (j != header_size)
                            std::cout << ", ";
                    }
                    std::cout << std::endl;
                }
            }
        }
    }
    return received_msg;
}

void process_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages) {
    for (size_t i = 0; i < inc_messages.size(); i++) {
        ReadMessage current_message = inc_messages.front();
        inc_messages.pop();
        std::cout << "Message: ";
        for (size_t j = 0; j < current_message.get_size(); j++) {
            std::cout << current_message.get_message()[j];
            if (j != current_message.get_size())
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
}
}  // namespace fes
}  // namespace mahi