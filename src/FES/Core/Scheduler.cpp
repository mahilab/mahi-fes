#include <Core/Scheduler.hpp>
#include <Core/Stimulator.hpp>
#include <MEL/Logging/Log.hpp>
#include <Utility/Utility.hpp>

using namespace mel;

namespace fes{
    Scheduler::Scheduler():
    id(0x01)
    {
    }

    Scheduler::~Scheduler(){
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

        if(write_message(hComm, crt_sched, "Creating Scheduler")){
            enable();
            sleep(setup_time);
            return true;
        }
        else{
            return false;
        }
    }

    bool Scheduler::add_event(Channel channel_, unsigned char event_type){

        if (enabled){
            for (size_t i = 0; i < events.size(); i++){
                if (events[i].get_channel() == channel_.get_channel_num()){
                    LOG(Error) << "Did not add event because an event already existed with that channel.";
                    return false;
                }
            }

            // add 5 us delay so that they don't all occur at the exact same time
            int delay_time = 5*events.size();           

            // 
            events.push_back(Event(hComm, id, delay_time, channel_));
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

            if(write_message(hComm, sync_msg1, "Sending Sync Message")){
                return true;
            }
            else{
                disable();
                return false;
            }
        }
        else{
            LOG(Error) << "Scheduler is not yet enabled";
        }
    }
        
    void Scheduler::enable(){
        enabled = true;
    }

    void Scheduler::disable(){
        enabled = false;

        unsigned char del_sched[] = { DEST_ADR,            // Destination
                                      SRC_ADR,             // Source  
                                      DELETE_SCHEDULE_MSG, // Msg type
                                      DEL_SCHED_LEN,       // Message length
                                      id,                  // Schedule ID
                                      0x00 };              // Checksum placeholder

        write_message(hComm, del_sched, "Closing Schedule");
    }

    unsigned char Scheduler::get_id(){
        return id;
    }
}
