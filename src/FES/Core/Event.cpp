#include "FES/Core/Event.hpp"
#include "FES/Core/Stimulator.hpp"
#include "FES/Utility/Utility.hpp"
#include "FES/Core/Channel.hpp"
#include "MEL/Logging/Log.hpp"

using namespace fes;
using namespace mel;

Event::Event(HANDLE& hComm_, unsigned char schedule_id_, int delay_time_, Channel channel_, int pulse_width_, int amplitude_, unsigned char event_type_, unsigned char priority_, unsigned char zone_):
    hComm(hComm_),
    schedule_id(schedule_id_),
    delay_time(delay_time_),
    channel(channel_),
    pulse_width(pulse_width_),
    amplitude(amplitude_),
    event_type(event_type_),
    priority(priority_),
    zone(zone_)
{
    create_event();
}

Event::~Event(){
    delete_event();
}

bool Event::create_event(){
    std::vector<unsigned char> delay_time_chars = int_to_twobytes(delay_time);
    
    unsigned char create_event[] = {DEST_ADR,                  // Destination
                                    SRC_ADR,                   // Source  
                                    CREATE_EVENT_MSG,          // Msg type
                                    CR_EVT_LEN,                // Message length
                                    schedule_id,               // Schedule ID  
                                    delay_time_chars[0],       // Delay time (byte 1)
                                    delay_time_chars[1],       // Delay time (byte 2)
                                    priority,                  // priority (default none)
                                    STIM_EVENT,                // Event type
                                    channel.get_channel_num(), // Channel number
                                    pulse_width,               // Pulse Width
                                    amplitude,                 // Amplitude
                                    zone,                      // Zone
                                    0x00};                     // Checksum Placeholder

    if(write_message(hComm, create_event, "Creating Event")){
        LOG(Info) << channel.get_name() << " Event Created.";
        return true;
    }
    else{
        LOG(Error) << "Error Creating " << channel.get_name() << " Event";
    }
}

bool Event::write_pulsewidth(){

}

bool Event::write_amplitude(){

}

bool Event::delete_event(){

}

unsigned char Event::get_channel(){
    return channel.get_channel_num();
}