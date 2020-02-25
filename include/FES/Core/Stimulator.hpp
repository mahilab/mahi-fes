#pragma once

#include <string>
#include <vector>
#include <FES/Core/Scheduler.hpp>
#include <FES/Core/Channel.hpp>
#include <FES/Utility/Utility.hpp>
#include <Windows.h>

namespace fes{
    class Stimulator {
    public:

        Stimulator(const std::string& name_, const std::string& com_port_, std::vector<Channel>& channels_, size_t size_);
        
        ~Stimulator();

        bool enable();

        void disable();

        std::string get_name();

        bool create_scheduler(const unsigned char sync_msg, unsigned int duration);

        bool is_enabled();

        void write_amp(Channel channel_, unsigned int amplitude_);

        void write_amps(std::vector<Channel> channels_, std::vector<int> amplitudes_);

        void write_pws(std::vector<Channel> channels_, std::vector<int> pulsewidths_);
        
        void write_pw(Channel channel_, unsigned int pw_);

        void update_max_amp(Channel channel_, unsigned int max_amp_);
        
        void update_max_pw(Channel channel_, unsigned int max_pw_);

        bool add_event(Channel channel_, unsigned char event_type = STIM_EVENT);

        bool add_events(std::vector<Channel> channels_, unsigned char event_type = STIM_EVENT);

        std::vector<Channel> get_channels();

        bool begin();

        bool update();

        bool halt_scheduler();

        size_t num_events;
        std::vector<int> amplitudes;
        std::vector<int> pulsewidths;
        std::vector<int> max_amplitudes;
        std::vector<int> max_pulsewidths;
        std::vector<std::string> channel_names;

    private:
        // variables for serial communication
        HANDLE hComm;
        DCB dcbSerialParams = {0};

        mel::Time delay_time = mel::milliseconds(50);

        std::string name;
        std::string com_port;
        bool enabled;
        std::vector<Channel> channels;
        Scheduler scheduler;
        mel::Time setup_time = mel::milliseconds(100);

        bool open_port();

        bool configure_port();
        
        bool initialize_board();
        
        void close_stimulator();
    };
}