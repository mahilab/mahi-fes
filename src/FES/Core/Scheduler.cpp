#include <Scheduler.hpp>

namespace fes{
    Scheduler::Scheduler(){

    }

    bool Scheduler::add_event(Channel channel_, unsigned char event_type, int amp, int pw){
        // do something
    }

    void Scheduler::enable(){
        enabled = true;
    }
    void Scheduler::set_id(unsigned char id_){
        id = id_;
    }
}
