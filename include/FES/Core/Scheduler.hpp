#pragma once

#include <vector>
#include <FES/Core/Channel.hpp>

namespace fes{
    class Scheduler
    {
    private:
        unsigned char id;
        std::vector<unsigned char> event_ids;
        bool enabled;
        HANDLE hComm;

        #define DEL_SCHED_LEN 0x01

    public:
        Scheduler(unsigned char id_);
        ~Scheduler();

        bool create_scheduler(HANDLE hComm_, const unsigned char sync_msg, unsigned int duration, Time setup_time);
        bool add_event(Channel channel_, unsigned char event_type, int amp, int pw);
        bool edit_event(Channel channel_);
        void enable();
        void disable();
        unsigned char get_id();
    };
}