#pragma once

#include <vector>
#include <FES/Core/Channel.hpp>
#include <FES/Core/Event.hpp>
#include <MEL/Core/Time.hpp>

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
        #define STIM_EVENT    0x03

    public:
        Scheduler();
        ~Scheduler();

        bool create_scheduler(HANDLE& hComm_, const unsigned char sync_msg, unsigned int duration, mel::Time setup_time);
        
        bool add_event(Channel channel_, unsigned char event_type = STIM_EVENT);

        bool edit_event(Channel channel_);
        
        void enable();
        
        void disable();
        
        unsigned char get_id();

        void write_amp(Channel channel_, unsigned int amplitude_);

        unsigned int get_amp(Channel channel_);
        
        void write_pw(Channel channel_, unsigned int pw_);

        unsigned int get_pw(Channel channel_);

        size_t get_num_events();

        bool halt_scheduler();

        bool update();
        
        bool send_sync_msg();

        bool is_enabled();
    };
}