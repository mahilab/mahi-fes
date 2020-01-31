#include <FES/Core/Channel.hpp>
#include <FES/Core/Stimulator.hpp>
#include <FES/Utility/Utility.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <Windows.h>
#include <tchar.h>

using namespace mel;

namespace fes{
    Channel::Channel(const std::string& name_, unsigned char channel_num_, int max_amp_, int max_pw_, int ip_delay_, unsigned char aspect_):
        name(name_),
        channel_num(channel_num_),
        max_amp(max_amp_),
        max_pw(max_pw_),
        ip_delay(ip_delay_),
        aspect(aspect_)
        {
            
        };

    bool Channel::setup_channel(HANDLE serial_handle_, Time delay_time_){
        DWORD dwBytesWritten = 0; // Captures how many bits were written
        
        if (ip_delay > 256){
            LOG(Error) << "ip_delay > 256 not yet supported. Please fix in Channel.cpp " << channel_num;
            return false;
        }

        //                                Destination Source   Msg type           MSG len     Channel      AmpLim   PWLim         IP delay  Aspect       Anode Cathode  Checksum
        unsigned char setup_message[] = { DEST_ADR,   SRC_ADR, CHANNEL_SETUP_MSG, CH_SET_LEN, channel_num, max_amp, max_pw, 0x00, ip_delay, ONE_TO_ONE,  AN_CA_1,       0x00 };  // channel 1 setup

        // Generate checksum in last index
        setup_message[(sizeof(setup_message) / sizeof(*setup_message)) - 1] = checksum(setup_message, (sizeof(setup_message) / sizeof(*setup_message)));

        // Attempt to send setup message and log whether it was successful or not
        if(!WriteFile(serial_handle_, setup_message, (sizeof(setup_message) / sizeof(*setup_message)),&dwBytesWritten,NULL)){
            LOG(Error) << "Error setting up Channel " << channel_num;
            return false;
        }
        else{
            LOG(Info) << "Channel " << channel_num << " Setup was Successful.";
        }

        // Sleep for delay time to allow the board to process
        mel::sleep(delay_time_);
        return true;
    }
}

