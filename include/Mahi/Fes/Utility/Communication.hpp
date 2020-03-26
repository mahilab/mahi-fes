#pragma once

#include <Windows.h>

#include <Mahi/Util.hpp>
#include <Mahi/Fes/Core/ReadMessage.hpp>
#include <queue>

namespace mahi {
namespace fes {
/// continues to read messages while messages are available and returns all read messages
std::vector<ReadMessage> get_all_messages(HANDLE hComm);
/// currently prints out all of the new incoming messages in a readable format to the commmand line
void process_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
/// reads a single message from the serial handle. 
std::vector<unsigned char> read_message(HANDLE hComm, bool should_wait, mahi::util::Time timeout = mahi::util::seconds(1));
}  // namespace fes
}  // namespace mahi