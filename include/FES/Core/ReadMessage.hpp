#include "FES/Core/Message.hpp"

namespace fes{

class ReadMessage : public Message
{
public:
    ReadMessage(std::vector<unsigned char> message);
    void set_message(std::vector<unsigned char> message);
private:
    void add_checksum();
};

} // namespace fes