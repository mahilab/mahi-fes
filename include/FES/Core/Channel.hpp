#pragma once

#include <Windows.h>
#include <string>

namespace fes{

    // definition of channel 1-4
    #define CH_1       0x00
    #define CH_2       0x01
    #define CH_3       0x02
    #define CH_4       0x03

    // definition of aspect ratios
    #define ONE_TO_ONE 0x11
    // #define TWO_TO_ONE 0x21

    class Channel
    {
    private:
        std::string name;
        unsigned char aspect;
        unsigned char channel_num;
        int max_amp;  // maximum amplitude
        int max_pw;   // maximum pulsewidth
        int ip_delay; // interphase delay
        unsigned char event_id;
    public:
        Channel(const std::string& name_, unsigned char channel_num_, int max_amp_, int max_pw_, int ip_delay_ = 100, unsigned char aspect_ = ONE_TO_ONE);
        ~Channel();

        bool setup_channel(HANDLE serial_handle_, Time delay_time_);

        unsigned char get_channel_num();

        std::string get_name();
    };
}
