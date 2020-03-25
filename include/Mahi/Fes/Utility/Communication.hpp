#pragma once

#include <Windows.h>

#include <Mahi/Util.hpp>
#include <Mahi/Fes/Core/ReadMessage.hpp>
#include <queue>

namespace mahi {
namespace fes {
/// continues to read messages and places them into inc_messages while messages are available
void check_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
/// waits for an incoming message, returns it once received. This has a timeout of 1 second,
/// and will return an empty message if none is received
ReadMessage wait_for_message(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
/// currently prints out all of the new incoming messages in a readable format to the commmand line
void process_inc_messages(HANDLE hComm, std::queue<ReadMessage> &inc_messages);
/// reads a single message from the serial handle. 
std::vector<unsigned char> read_message(HANDLE hComm, bool should_wait, mahi::util::Time timeout = mahi::util::seconds(1));
}  // namespace fes
}  // namespace mahi