#pragma once

#include "FES/Core/Stimulator.hpp"
#include "FES/Core/Channel.hpp"

namespace fes{
    class Event{
    private:
        HANDLE hComm;
        unsigned char schedule_id;
        int delay_time;
        Channel channel;
        int pulse_width;
        int amplitude;
        unsigned char event_type;
        unsigned char priority;
        unsigned char zone;
    public:

        #define STIM_EVENT 0x03

        Event(HANDLE& hComm, unsigned char schedule_id_, int delay_time_, Channel channel_, int pulse_width_ = 0, int amplitude_ = 0, unsigned char event_type_ = STIM_EVENT, unsigned char priority_ = 0x00, unsigned char zone_ = 0x00);
        ~Event();
        bool Event::create_event();

        unsigned char get_channel();

        bool Event::edit_event();

        bool Event::write_pulsewidth();
        
        bool Event::write_amplitude();

        bool Event::delete_event();
    };
}



