#include <FES/Utility/Utility.hpp>
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
    channels.push_back(bicep);

    Channel tricep ("tricep",  CH_2, 100, 250);
    channels.push_back(tricep);

    Channel forearm("forearm", CH_3, 100, 250);
    channels.push_back(forearm);
    
    Channel wrist  ("wrist",   CH_4, 100, 250);
    channels.push_back(wrist);
    
    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", "COM5", channels, channels.size());

    // Initialize scheduler with the sync character and duration of scheduler in ms
    stim.create_scheduler(0xAA, 25);

    // Input which events will be added to the scheduler for updates
    stim.add_events(channels);

    // Initialize a timer for how often to update
    Timer timer(milliseconds(1), Timer::WaitMode::Hybrid);
    timer.set_acceptable_miss_rate(0.05);

    // start sending stimulation to the board
    stim.begin();

    while(!stop){

        // update the pulsewidth of each of the stimulation events
        stim.write_pw(bicep,0);
        stim.write_pw(tricep,0);
        stim.write_pw(forearm,0);
        stim.write_pw(wrist,0);

        // command the stimulation patterns to be sent to the stim board
        stim.update();
    }

    // disable events, schedulers, boards, etc
    stim.disable();


    return 0;
}
