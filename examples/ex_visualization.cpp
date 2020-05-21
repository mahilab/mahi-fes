// #define MAHI_GUI_NO_CONSOLE
#include <Mahi/Fes.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <atomic>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>

using namespace mahi::util;
using namespace mahi::gui;
using namespace mahi::fes;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool      handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main(int argc, char const *argv[]) {
    // register ctrl-c handler
    register_ctrl_handler(handler);

    // create channels of interest
    std::vector<Channel> channels;

    // create new channel for bicep (Channel 1, Anode Cathode Pair 1, max amp 100 mA, max pw 250 us)
    Channel bicep("Bicep", CH_1, AN_CA_1, 100, 250);
    channels.push_back(bicep);

    // create new channel for tricep (Channel 2, Anode Cathode Pair 2, max amp 100 mA, max pw 250 us)
    Channel tricep("Tricep", CH_2, AN_CA_2, 100, 250);
    channels.push_back(tricep);

    // create new channel for pronator teres (Channel 3, Anode Cathode Pair 3, max amp 100 mA, max pw 250 us)
    Channel pt("Pronator Teres", CH_3, AN_CA_3, 100, 250);
    channels.push_back(pt);

    // create new channel for brachioradialis (Channel 4, Anode Cathode Pair 4, max amp 100 mA, max pw 250 us)
    Channel br("Brachioradialis", CH_4, AN_CA_4, 100, 250);
    channels.push_back(br);

    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", channels, "COM11");

    // Initialize scheduler with the sync character and frequency of scheduler in hertz
    stim.create_scheduler(0xAA, 100);

    // Input which events will be added to the scheduler for updates
    stim.add_events(channels);

    // start the visualization thread to run the gui. This is optional, but allows the stimulators to be updated through
    // the gui rather than in code. The code will overwrite the gui, so if gui control is desired, remove updates in the
    // while loop below
    std::thread viz_thread([&stim]() {
        Visualizer visualizer(&stim);
        visualizer.run();
    });

    // start sending stimulation to the board
    stim.begin();

    // set timer for our control loop
    Timer timer(milliseconds(50), Timer::WaitMode::Hybrid);
    timer.set_acceptable_miss_rate(0.05);
    
    // variable to keep track of our current time
    double t(0.0);

    while (true) {
        stim.update();
        t = timer.wait().as_seconds();
    }

    viz_thread.join();

    return 0;
}
