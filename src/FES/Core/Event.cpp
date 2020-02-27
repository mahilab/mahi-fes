#include "FES/Core/Event.hpp"
#include "FES/Core/Stimulator.hpp"
#include "FES/Utility/Utility.hpp"
#include "FES/Core/Channel.hpp"
#include "MEL/Logging/Log.hpp"

using namespace mel;
namespace fes{

    Event::Event(HANDLE& hComm_, unsigned char schedule_id_, int delay_time_, Channel channel_, unsigned char event_id_, unsigned int pulse_width_, unsigned int amplitude_, unsigned char event_type_, unsigned char priority_, unsigned char zone_):
        hComm(hComm_),
        schedule_id(schedule_id_),
        delay_time(delay_time_),
        channel(channel_),
        pulse_width(pulse_width_),
        amplitude(amplitude_),
        event_type(event_type_),
        priority(priority_),
        event_id(event_id_),
        max_amplitude(channel.get_max_amplitude()),
        max_pulse_width(channel.get_max_pulse_width()),
        zone(zone_)
    {
        create_event();
    }

    Event::~Event(){
        
    }

    bool Event::create_event(){
        std::vector<unsigned char> delay_time_chars = int_to_twobytes(delay_time);
        
        unsigned char create_event[] = {DEST_ADR,                   // Destination
                                        SRC_ADR,                    // Source  
                                        CREATE_EVENT_MSG,           // Msg type
                                        CR_EVT_LEN,                 // Message length
                                        schedule_id,                // Schedule ID  
                                        delay_time_chars[0],        // Delay time (byte 1)
                                        delay_time_chars[1],        // Delay time (byte 2)
                                        priority,                   // priority (default none)
                                        STIM_EVENT,                 // Event type
                                        channel.get_channel_num(),  // Channel number
                                        (unsigned char)pulse_width, // Pulse Width
                                        (unsigned char)amplitude,   // Amplitude
                                        zone,                       // Zone
                                        0x00};                      // Checksum Placeholder

        if(write_message(hComm, create_event, sizeof(create_event)/sizeof(*create_event), "Creating Event")){
            return true;
        }
        else{
            return false;
        }
    }

    void Event::set_amplitude(unsigned int amplitude_){
        if (amplitude_ > max_amplitude){
            amplitude = max_amplitude;
            LOG(Warning) << "Commanded too high of a amplitude on " << get_channel_name() << " channel. It was clamped to " << max_amplitude << ".";
        }
        else if (amplitude_ < 0){
            amplitude = 0;
            LOG(Warning) << "Commanded a negative amplitude on " << get_channel_name() << " channel. It was clamped to 0.";
        }
        else{
            amplitude = amplitude_;
        }
    }

    unsigned int Event::get_amplitude(){
        return amplitude;
    }

    void Event::set_pulsewidth(unsigned int pulsewidth_){
        if (pulsewidth_ > max_pulse_width){
            pulse_width = max_pulse_width;
            LOG(Warning) << "Commanded too high of a pulsewidth on " << get_channel_name() << " channel. It was clamped to " << max_pulse_width << ".";
        }
        else if (pulsewidth_ < 0){
            pulse_width = 0;
            LOG(Warning) << "Commanded a negative pulsewidth on " << get_channel_name() << " channel. It was clamped to 0.";
        }
        else{
            pulse_width = pulsewidth_;
        }
    }

    unsigned int Event::get_pulsewidth(){
        return pulse_width;
    }

    bool Event::update(){
        unsigned char edit_event_msg[] = {DEST_ADR,                  // Destination
                                         SRC_ADR,                    // Source
                                         CHANGE_EVENT_PARAMS_MSG,    // Msg type
                                         CHANGE_EVENT_PARAMS_LEN,    // Msg len
                                         event_id,                   // Event ID
                                         (unsigned char)pulse_width, // Pulsewidth to update
                                         (unsigned char)amplitude,   // Amplitude to update
                                         0x00,                       // Placeholder for other parameters
                                         0x00};                      // Checksum placeholder

        std::cout << (unsigned int)event_id << std::endl;

        if(write_message(hComm, edit_event_msg, sizeof(edit_event_msg)/sizeof(*edit_event_msg), "NONE")){
            return true;
        }
        else{
            return false;
        }        
    }

    bool Event::delete_event(){
        unsigned char del_evt_message[] = { DEST_ADR,         // Destination
                                            SRC_ADR,          // Source
                                            DELETE_EVENT_MSG, // Msg type
                                            DELETE_EVENT_LEN, // Msg len
                                            event_id,         // Event ID
                                            0x00 };           // Checksum placeholder

        if(write_message(hComm, del_evt_message, sizeof(del_evt_message)/sizeof(*del_evt_message), "Deleting Event")){
            return true;
        }
        else{
            return false;
        }        
    }

    unsigned char Event::get_channel_num(){
        return channel.get_channel_num();
    }
    
    std::string Event::get_channel_name(){
        return channel.get_channel_name();
    }
}
