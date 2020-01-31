#include <vector>
#include <FES/Core/Channel.hpp>
#pragma once

namespace fes{
    class Scheduler
    {
    private:
        unsigned char id;
        std::vector<unsigned char> event_ids;
        bool enabled;

    public:
        Scheduler();
        ~Scheduler();

        bool add_event(Channel channel_, unsigned char event_type, int amp, int pw);
        void enable();
        void set_id(unsigned char id_);
    };
}