#pragma once

#include <vector>
#include <FES/Core/Channel.hpp>
#include <FES/Core/Event.hpp>

namespace fes{
    class Scheduler{
    private:
        unsigned char id;
        std::vector<unsigned char> event_ids;
        std::vector<Event> events;
        bool enabled;
        HANDLE hComm;
        unsigned char sync_char;

        #define DEL_SCHED_LEN 0x01

    public:
        Scheduler();
        ~Scheduler();

        bool create_scheduler(HANDLE& hComm_, const unsigned char sync_msg, unsigned int duration, Time setup_time);
        bool add_event(Channel channel_, unsigned char event_type);
        bool edit_event(Channel channel_);
        void enable();
        void disable();
        unsigned char get_id();
        bool send_sync_msg();
    };
}