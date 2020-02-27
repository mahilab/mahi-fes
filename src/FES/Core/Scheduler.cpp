#include <FES/Core/Scheduler.hpp>
#include <MEL/Logging/Log.hpp>
#include <FES/Utility/Utility.hpp>
#include <MEL/Core/Timer.hpp>

using namespace mel;

namespace fes{
    Scheduler::Scheduler():
    id(0x01)
    {
    }

    Scheduler::~Scheduler(){
        for (auto event = events.begin(); event != events.end(); event++){
            event->delete_event();
        }
        disable();
    }

    bool Scheduler::create_scheduler(HANDLE& hComm_, const unsigned char sync_char_, unsigned int duration, Time setup_time){
        sync_char = sync_char_;
        
        hComm = hComm_;

        DWORD dwBytesWritten = 0; // Captures how many bits were written

        // convert the input duration (int) into two bytes that we can send over a message
        std::vector<unsigned char> duration_chars = int_to_twobytes(duration);

        
        unsigned char crt_sched[] = { DEST_ADR,            // Destination
                                      SRC_ADR,             // Source  
                                      CREATE_SCHEDULE_MSG, // Msg type
                                      CREATE_SCHED_LEN,    // Message length
                                      sync_char,           // sync character  
                                      duration_chars[0],   // schedule duration (byte 1)  
                                      duration_chars[1],   // schedule duration (byte 2)
                                      0x00 };              // checksum placeholder

        if(write_message(hComm, crt_sched, sizeof(crt_sched)/sizeof(*crt_sched), "Creating Scheduler")){
            enabled = true;
            sleep(setup_time);
            return true;
        }
        else{
            return false;
        }
    }

    bool Scheduler::halt_scheduler(){
        if(is_enabled()){
            unsigned char halt_message[] = {DEST_ADR, // Destination
                                            SRC_ADR,  // Source
                                            HALT_MSG, // Msg type
                                            HALT_LEN, // Msg len
                                            id,       // Schedule ID
                                            0x00};    // Checksum placeholder

            return write_message(hComm, halt_message, sizeof(halt_message)/sizeof(*halt_message), "Schedule Closing");
        }
        else{
            LOG(Error) << "Scheduler was not enabled. Nothing to disable";
            return false;
        }
    }

    bool Scheduler::add_event(Channel channel_, unsigned char event_type){

        auto num_events = events.size();

        if (enabled){
            for (auto i = 0; i < num_events; i++){
                if (events[i].get_channel_num() == channel_.get_channel_num()){
                    LOG(Error) << "Did not add event because an event already existed with that channel.";
                    return false;
                }
            }

            // add 5 us delay so that they don't all occur at the exact same time
            auto delay_time = 5; // ms
            
            // add event to list of events
            events.push_back(Event(hComm, id, delay_time, channel_, (unsigned char)(num_events+1)));
            
            return true;
        }
        else{
            LOG(Error) << "Scheduler is not yet enabled. ";
            return false;
        }
    }

    bool Scheduler::send_sync_msg(){
        if (enabled){
            unsigned char sync_msg1[] = { DEST_ADR,     // Destination
                                          SRC_ADR,      // Source  
                                          SYNC_MSG,     // Msg type
                                          SYNC_MSG_LEN, // Message length
                                          sync_char,    // sync character
                                          0x00 };       // Checksum placeholder

            if(write_message(hComm, sync_msg1, sizeof(sync_msg1)/sizeof(*sync_msg1), "Sending Sync Message")){
                return true;
            }
            else{
                disable();
                return false;
            }
        }
        else{
            LOG(Error) << "Scheduler is not yet enabled";
            return false;
        }
    }

    void Scheduler::disable(){
        enabled = false;

        unsigned char del_sched[] = { DEST_ADR,            // Destination
                                      SRC_ADR,             // Source  
                                      DELETE_SCHEDULE_MSG, // Msg type
                                      DEL_SCHED_LEN,       // Message length
                                      id,                  // Schedule ID
                                      0x00 };              // Checksum placeholder

        write_message(hComm, del_sched, sizeof(del_sched)/sizeof(*del_sched), "Closing Schedule");
    }

    void Scheduler::write_amp(Channel channel_, unsigned int amplitude_){
        // loop over available events in the scheduler
        for (auto event = events.begin(); event != events.end(); event++){
            // if the event is for the correct channel we are looking for, write the amplitude and exit the function
            if (event->get_channel_num()==channel_.get_channel_num()){
                event->set_amplitude(amplitude_);
                return;
            }
        }
        // if we didnt find the event, something is messed up, so return false
        LOG(Error) << "Did not find the correct event to update. Nothing has changed.";
    }

    unsigned int Scheduler::get_amp(Channel channel_){
        // loop over available events in the scheduler
        for (auto event = events.begin(); event != events.end(); event++){
            // if the event is for the correct channel we are looking for, wrevente the ampleventude and exevent the function
            if (event->get_channel_num()==channel_.get_channel_num()){
                return event->get_amplitude();
            }
        }
        // if we didnt find the event, something is messed up, so return 0
        LOG(Error) << "Did not find the correct event to pull from. Returning 0.";
        return 0;
    }
        
    void Scheduler::write_pw(Channel channel_, unsigned int pw_){
        // loop over available events in the scheduler
        for (auto event = events.begin(); event != events.end(); event++){
            // if the event is for the correct channel we are looking for, wrevente the ampleventude and exevent the function
            if (event->get_channel_num()==channel_.get_channel_num()){
                event->set_pulsewidth(pw_);
                return;
            }
        }
        // if we didnt find the event, something is messed up, so return false
        LOG(Error) << "Did not find the correct event to update. Nothing has changed.";
    }

    unsigned int Scheduler::get_pw(Channel channel_){
        // loop over available events in the scheduler
        for (auto event = events.begin(); event != events.end(); event++){
            // if the event is for the correct channel we are looking for, wrevente the ampleventude and exevent the function
            if (event->get_channel_num()==channel_.get_channel_num()){
                return event->get_pulsewidth();
            }
        }
        // if we didnt find the event, something is messed up, so return 0
        LOG(Error) << "Did not find the correct event to pull from. Returning 0.";
        return 0;
    }

    bool Scheduler::update(){
        // loop over available events in the scheduler
        for (auto event = events.begin(); event != events.end(); event++){
            // If any channel fails to update, return false after throwing an error
            if(!event->update()){
                LOG(Error) << "Channel " << event->get_channel_name() << " failed to update";
                return false;
            }
        }
        return true;
    }

    size_t Scheduler::get_num_events(){
        return events.size();
    }

    unsigned char Scheduler::get_id(){
        return id;
    }

    bool Scheduler::is_enabled(){
        return enabled;
    }
}
