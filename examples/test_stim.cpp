#include <FES/Utility/Utility.hpp>
#include <FES/Core/Stimulator.hpp>
#include <FES/Core/Event.hpp>
#include <Mahi/Util.hpp>
#include <thread>
#include <mutex>
#include <FES/Utility/Visualizer.hpp>
#include <FES/Utility/VirtualStim.hpp>

using namespace mahi::util;
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

    Channel bicep  ("bicep",   CH_1, AN_CA_1, 100, 250);
    channels.push_back(bicep);

    Channel tricep ("tricep",  CH_2, AN_CA_2, 100, 250);
    channels.push_back(tricep);

    Channel forearm("forearm", CH_3, AN_CA_3, 100, 250);
    channels.push_back(forearm);
    
    Channel wrist  ("wrist",   CH_4, AN_CA_4, 100, 250);
    channels.push_back(wrist);
    
    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", "COM9", channels, channels.size());

    // Initialize scheduler with the sync character and frequency of scheduler in hertz
    stim.create_scheduler(0xAA, 100);

    // Input which events will be added to the scheduler for updates
    stim.add_events(channels);

    // Initialize a timer for how often to update

    double t;

    std::thread viz_thread([&stim](){
        Visualizer visualizer(&stim);
        visualizer.run();
    });

    // VirtualStim vstim("COM10");
    // vstim.begin();

    // start sending stimulation to the board
    stim.begin();
    // double t_last = 0.0;

    Timer timer(milliseconds(50), Timer::WaitMode::Hybrid);
    timer.set_acceptable_miss_rate(0.05);

    while(!stop){
        {
            // std::lock_guard<std::mutex> lock(mtx);
            // update the pulsewidth of each of the stimulation events
            stim.write_amp(bicep,40+int(10*sin(t)));
            stim.write_amp(tricep,30+int(10*sin(t)));
            stim.write_amp(forearm,20+int(10*sin(t)));
            stim.write_amp(wrist,10+int(10*sin(t)));

            // command the stimulation patterns to be sent to the stim board

            stim.update();

        }
        t = timer.wait().as_seconds();
        // mel::print(t-t_last);
        // t_last = t;
    }

    // disable events, schedulers, boards, etc
    stim.disable();

    // viz_thread.join();

    return 0;
}
