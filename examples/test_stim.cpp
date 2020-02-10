#include <FES/Core/Stimulator.hpp>
#include <FES/Core/Muscle.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Core/Timer.hpp>


using namespace mel;
using namespace fes;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main() {
    // register ctrl-c handler
    register_ctrl_handler(handler);

    // create channels of interest
    std::vector<Channel> channels;

    Channel bicep  ("bicep",   CH_1, 100, 250);
    Channel tricep ("tricep",  CH_2, 100, 250);
    Channel forearm("forearm", CH_3, 100, 250);
    Channel wrist  ("wrist",   CH_4, 100, 250);
    channels.push_back(bicep);
    channels.push_back(tricep);
    channels.push_back(forearm);
    channels.push_back(wrist);

    // Create stim board
    Stimulator stim("UECU Board", "COM5", channels);

    //                    sync  dur
    stim.create_scheduler(0xAA, 25);

    //             channel
    stim.add_event(bicep);
    stim.add_event(tricep);
    stim.add_event(forearm);
    stim.add_event(wrist);

    Timer timer(milliseconds(1), Timer::WaitMode::Hybrid);
    timer.set_acceptable_miss_rate(0.05);

    stim.begin();
    while(!stop){
        stim.write_pw(bicep,0);
        stim.write_pw(tricep,0);
        stim.write_pw(forearm,0);
        stim.write_pw(wrist,0);

        stim.update();
    }

    stim.disable();


    return 0;
}
