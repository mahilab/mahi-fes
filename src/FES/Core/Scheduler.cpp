#include <Core/Scheduler.hpp>
#include <Core/Stimulator.hpp>
#include <MEL/Logging/Log.hpp>
#include <Utility/Utility.hpp>

using namespace mel;

namespace fes{
    Scheduler::Scheduler(unsigned char id_):
    id(id_)
    {
    }

    Scheduler::~Scheduler(){
        disable();
    }

    bool Scheduler::create_scheduler(HANDLE hComm_, const unsigned char sync_msg, unsigned int duration, Time setup_time){
        hComm = hComm_;

        DWORD dwBytesWritten = 0; // Captures how many bits were written

        // convert the input duration (int) into two bytes that we can send over a message
        std::vector<unsigned char> duration_chars = int_to_twobytes(duration);

        //                            Destination Source   Msg type             Message length    sync msg  
        unsigned char crt_sched[] = { DEST_ADR,   SRC_ADR, CREATE_SCHEDULE_MSG, CREATE_SCHED_LEN, sync_msg, duration_chars[0], duration_chars[1], 0x00 };

        if(!WriteFile(hComm, crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched)),&dwBytesWritten,NULL)){
            LOG(Error) << "Error in Scheduler Setup.";
            return false;
        }
        else{
            LOG(Info) << "Scheduler Setup was Successful.";
        }

        enable();
        
        sleep(setup_time);
        return true;
    }

    bool Scheduler::add_event(Channel channel_, unsigned char event_type, int amp, int pw){
        // do something
    }

    void Scheduler::enable(){
        enabled = true;
    }

    void Scheduler::disable(){
        enabled = false;

        DWORD dwBytesWritten = 0; // Captures how many bits were written

        unsigned char del_sched[] = { DEST_ADR,   SRC_ADR, DELETE_SCHEDULE_MSG, DEL_SCHED_LEN, id, 0x00 };

        if(!WriteFile(hComm, del_sched, (sizeof(del_sched) / sizeof(*del_sched)),&dwBytesWritten,NULL)){
            LOG(Error) << "Error Closing Schedule.";
        }
        else{
            LOG(Info) << "Schedule Closing was Successful.";
        }
    }

    unsigned char Scheduler::get_id(){
        return id;
    }
}
