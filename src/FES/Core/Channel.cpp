#include <FES/Core/Channel.hpp>
#include <FES/Utility/Utility.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <Windows.h>
#include <tchar.h>

using namespace mel;

namespace fes{

    Channel::Channel(const std::string& name_, unsigned char channel_num_, unsigned char an_ca_nums_, unsigned int max_amp_, unsigned int max_pw_, unsigned int ip_delay_, unsigned char aspect_):
        name(name_),
        channel_num(channel_num_),
        an_ca_nums(an_ca_nums_),
        max_amp(max_amp_),
        max_pw(max_pw_),
        ip_delay(ip_delay_),
        aspect(aspect_)
        {
            
        };
    
    Channel::~Channel(){

    }

    bool Channel::setup_channel(HANDLE serial_handle_, Time delay_time_){

        std::vector<unsigned char> ip_delay_bytes = int_to_twobytes(ip_delay);
        
        unsigned char setup_message[] = { DEST_ADR,               // Destination
                                          SRC_ADR,                // Source
                                          CHANNEL_SETUP_MSG,      // Msg type
                                          CH_SET_LEN,             // Msg len
                                          channel_num,            // Channel
                                          (unsigned char)max_amp, // AmpLim
                                          (unsigned char)max_pw,  // PWLim 
                                          ip_delay_bytes[0],      // IP delay (byte 1)
                                          ip_delay_bytes[1],      // IP delay (byte 2)
                                          ONE_TO_ONE,             // Aspect
                                          an_ca_nums,                // Anode Cathode
                                          0x00 };                 // Checksum

        if(write_message(serial_handle_, setup_message, sizeof(setup_message)/sizeof(*setup_message), "Setting Up Channel")){
            // Sleep for delay time to allow the board to process
            mel::sleep(delay_time_);
            return true;
        }
        else{
            return false;
        }        
    }

    unsigned int Channel::get_max_amplitude(){
        return max_amp;
    }

    unsigned int Channel::get_max_pulse_width(){
        return max_pw;
    }

    void Channel::set_max_amplitude(unsigned int max_amp_){
        max_amp = max_amp_;
    }

    void Channel::set_max_pulse_width(unsigned int max_pw_){
        max_pw = max_pw_;
    }

    unsigned char Channel::get_channel_num(){
        return channel_num;
    }

    std::string Channel::get_channel_name(){
        return name;
    }
}

