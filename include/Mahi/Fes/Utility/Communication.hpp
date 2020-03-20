# pragma once

#include <Mahi/Fes/Core/ReadMessage.hpp>
#include <queue>
#include <Windows.h>

namespace mahi {
namespace fes {
void        check_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
ReadMessage wait_for_message(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
void        process_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
}  // namespace fes
}  // namespace mahi