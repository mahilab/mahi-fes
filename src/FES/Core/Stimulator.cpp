#include <FES/Core/Stimulator.hpp>
#include <FES/Core/Channel.hpp>
#include <FES/Core/Event.hpp>
#include <FES/Utility/Utility.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Utility/System.hpp>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <codecvt>
#include <locale>

using namespace mel;

namespace fes{
    Stimulator::Stimulator(const std::string& name_, const std::string& com_port_, std::vector<Channel>& channels_, size_t size_):
        name(name_),
        com_port(com_port_),
        enabled(false),
        channels(channels_),
        scheduler(),
        num_events(size_),
        amplitudes(num_events,0),
        pulsewidths(num_events,0),
        max_amplitudes(num_events,0),
        max_pulsewidths(num_events,0)
    {
        print(num_events);
        for (auto i = 0; i < num_events; i++){
            max_amplitudes[i] = channels[i].get_max_amplitude();
            max_pulsewidths[i] = channels[i].get_max_pulse_width();
            channel_names.push_back(channels[i].get_channel_name());
        }        
        enable();
    }

    Stimulator::~Stimulator(){
        disable();
    }

    // Open and configure serial port, and initialize the channels on the board.
    bool Stimulator::enable() {
        // open the comport with read/write permissions
        if(!open_port()){
            enabled = false;
            return enabled;
        }
        // Configure the parameters for serial port ttyUSB0
        if(!configure_port()){
            enabled = false;
            return enabled;
        }   
        // Write stim board setup commands to serial port ttyUSB0
        if(!initialize_board()){
            enabled = false;
            return enabled;
        } 
        enabled = true;
        return enabled;
    }

    void Stimulator::disable(){
        if(is_enabled()){
            close_stimulator();
            LOG(Info) << "Stimulator Disabled";
        }
        else{
            LOG(Info) << "Stimulator has not been enabled yet.";
        }
        enabled = false;
    }

    bool Stimulator::open_port(){

        // the comport must be formatted as an LPCWSTR, so we need to get it into that form from a std::string
        std::wstring com_prefix = L"\\\\.\\";
        std::wstring com_suffix = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(com_port);
        std::wstring comID = com_prefix + com_suffix;
        
        // std::wstring stemp = std::wstring(com_port_formatted.begin(), com_port_formatted.end());
        // LPCWSTR com_port_lpcwstr = stemp.c_str();

        hComm = CreateFileW(comID.c_str(),                // port name
                            GENERIC_READ | GENERIC_WRITE, // Read/Write
                            0,                            // No Sharing
                            NULL,                         // No Security
                            OPEN_EXISTING,                // Open existing port only
                            0,                            // Non Overlapped I/O
                            NULL);                        // Null for Comm Devices

        // Check if creating the comport was successful or not and log it
        if (hComm == INVALID_HANDLE_VALUE){
            LOG(Error) << "Failed to open Stimulator" << get_name();
            return false;
        }
        else{
            LOG(Info) << "Opened Stimulator" << get_name();
        }

        return true;
    }

    bool Stimulator::configure_port(){  // configure_port establishes the settings for each serial port

        // http://bd.eduweb.hhs.nl/micprg/pdf/serial-win.pdf

        dcbSerialParams.DCBlength = sizeof(DCB);

        if (!GetCommState(hComm, &dcbSerialParams)) {
            LOG(Error) << "Error getting serial port state";
            return false;
        }

        // set parameters to use for serial communication

        // set the baud rate that we will communicate at to 9600
        dcbSerialParams.BaudRate = CBR_9600;

        // 8 bits in the bytes transmitted and received.
        dcbSerialParams.ByteSize = 8;
        
        // Specify that we are using one stop bit
        dcbSerialParams.StopBits = ONESTOPBIT;

        // Specify that we are using no parity 
        dcbSerialParams.Parity = NOPARITY;

        // Disable all parameters dealing with flow control
        dcbSerialParams.fOutX = FALSE;
        dcbSerialParams.fInX  = FALSE;
        dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
        dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
        // dcbSerialParams.fOutxCtsFlow = FALSE;
        // dcbSerialParams.fOutxDsrFlow = FALSE;

        // Set communication parameters for the serial port
        if(!SetCommState(hComm, &dcbSerialParams)){
            LOG(Error) << "Error setting serial port state";
            return false;
        }

        COMMTIMEOUTS timeouts={0};
        timeouts.ReadIntervalTimeout=50;
        timeouts.ReadTotalTimeoutConstant=50;
        timeouts.ReadTotalTimeoutMultiplier=10;
        timeouts.WriteTotalTimeoutConstant=50;
        timeouts.WriteTotalTimeoutMultiplier=10;
        if(!SetCommTimeouts(hComm, &timeouts)){
            LOG(Error) << "Error setting serial port timeouts";
            return false;
        }

        return true;
    }

    bool Stimulator::initialize_board(){

        // delay time after sending setup messages of serial comm
        
        //Create byte array
        //Structure of byte arrays:
        //  Destination address - always 0x04
        //  Source address - always 0x80
        //  Message type - see message commands in header file
        //  Message Length - length of message without header (everything up to msg length) or checksum
        //  Message - Bytes of the message - must be of length Message Length
        //  Checksum Calculation - add each of the bytes, mask the lower byte of sum and add cary byte, then invert sum

        //Structure of channel setup messages:
        //  1 byte:  Port channel - lower 4 bits are channel, upper for bits are port (always 0): aka always 0x0a where a is channel number
        //  1 byte:  Amplitude limit - max amplitude the channel will outport
        //  1 byte:  Pulse Width Limit - max pulse with that channel will output
        //  2 bytes: Interphase Delay in usec - space between phases of the waveform (can be from 10-65535)
        //  1 byte:  Aspect Ratio - designates the proportion of the amplitude of the first phase to the second phase.
        //             - The lower 4 bits represent the first phase and the upper 4 bits represent the second phase. 0x11 is 1 to 1 ratio
        //  1 byte:  AnodeCathode - for 4 bipolar channels (what we have) these are 0x01, 0x23, 0x45, 0x67 respectively
        
        for (auto i = 0; i < channels.size(); i++){
            if (!channels[i].setup_channel(hComm, delay_time)){
                return false;
            };
        }

        LOG(Info) << "Setup Completed successfully.";

        return true;
    }

    bool Stimulator::halt_scheduler(){
        return scheduler.halt_scheduler();
    }

    void Stimulator::close_stimulator(){
        CloseHandle(hComm);     
        enabled = false;
    }

    bool Stimulator::begin(){
        if (is_enabled()){
            enabled = true;
            return scheduler.send_sync_msg();
        }
        else{
            LOG(Error) << "Stimulator has not yet been opened. Not starting the stimulator";
            return false;
        }
    }

    void Stimulator::write_amp(Channel channel_, unsigned int amp_){
        if (is_enabled()){
            scheduler.write_amp(channel_, amp_);
        }
        else{
            LOG(Error) << "Stimulator has not yet been enabled. Not writing amplitude";
        }
    }

    void Stimulator::write_amps(std::vector<Channel> channels_, std::vector<int> amplitudes_){
        for (size_t i = 0; i < channels_.size(); i++){
            write_amp(channels_[i], amplitudes_[i]);
        }
    }

    void Stimulator::write_pws(std::vector<Channel> channels_, std::vector<int> pulsewidths_){
        for (size_t i = 0; i < channels_.size(); i++){
            write_pw(channels_[i], pulsewidths_[i]);
        }
    }

    void Stimulator::write_pw(Channel channel_, unsigned int pw_){
        if (is_enabled()){
            scheduler.write_pw(channel_, pw_);
        }
        else{
            LOG(Error) << "Stimulator has not yet been enabled. Not writing pulsewidth";
        }
    }

    void Stimulator::update_max_amp(Channel channel_, unsigned int max_amp_){
        for (auto channel = channels.begin(); channel != channels.end(); channel++){
            // if the channel is the correct channel we are looking for, set the max amplitude of the channel
            if (channel->get_channel_name()==channel_.get_channel_name()){
                channel->set_max_amplitude(max_amp_);
                return;
            }
        }
        LOG(Error) << "Did not find the correct channel to update";
    }
    
    void Stimulator::update_max_pw(Channel channel_, unsigned int max_pw_){
        for (auto channel = channels.begin(); channel != channels.end(); channel++){
            // if the channel is the correct channel we are looking for, set the max pulsewidth of the channel
            if (channel->get_channel_name()==channel_.get_channel_name()){
                channel->set_max_pulse_width(max_pw_);
                return;
            }
        }
        LOG(Error) << "Did not find the correct channel to update";
    }

    bool Stimulator::update(){
        if (is_enabled()){
            amplitudes.resize(num_events);
            pulsewidths.resize(num_events);
            max_amplitudes.resize(num_events);
            max_pulsewidths.resize(num_events);
            for (size_t i = 0; i < scheduler.get_num_events(); i++){
                amplitudes[i]  = scheduler.get_amp(channels[i]);
                pulsewidths[i] = scheduler.get_pw(channels[i]);
                max_amplitudes[i] = channels[i].get_max_amplitude();
                max_pulsewidths[i] = channels[i].get_max_pulse_width();
            }
            return scheduler.update();
        }
        else{
            LOG(Error) << "Stimulator has not yet been enabled. Not updating";
            return false;
        }
    }

    bool Stimulator::create_scheduler(const unsigned char sync_msg , unsigned int duration){
        if (is_enabled()){
            return scheduler.create_scheduler(hComm, sync_msg, duration, delay_time);
        }
        else{
            LOG(Error) << "Stimulator has not yet been enabled. Not creating scheduler";
            return false;
        }
    }

    bool Stimulator::add_event(Channel channel_, unsigned char event_type){
        if (is_enabled()){
            return scheduler.add_event(channel_, event_type);
        }
        else{
            LOG(Error) << "Stimulator has not yet been enabled. Not adding event to scheduler";
            return false;
        }
    }

    bool Stimulator::add_events(std::vector<Channel> channels_, unsigned char event_type){
        if (is_enabled()){
            for (size_t i = 0; i < channels_.size(); i++){
                // If any channel fails to add, return false after throwing an error
                if(!add_event(channels_[i], event_type)){
                    return false;
                };
            }
            return true;
        }
        else{
            LOG(Error) << "Stimulator has not yet been enabled. Not adding event to scheduler";
            return false;
        }
    }

    std::vector<Channel> Stimulator::get_channels(){
        return channels;
    }

    bool Stimulator::is_enabled(){
        return enabled;
    }

    std::string Stimulator::get_name(){
        return name;
    }
}