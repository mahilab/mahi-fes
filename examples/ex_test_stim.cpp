#include <Mahi/Fes.hpp>
#include <Mahi/Util.hpp>
#include <mutex>
#include <thread>

using namespace mahi::util;
using namespace mahi::fes;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool      handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main() {
    // register ctrl-c handler
    register_ctrl_handler(handler);

    // create channels of interest
    std::vector<Channel> channels;

    Channel bicep("bicep", CH_1, AN_CA_1, 100, 250);
    channels.push_back(bicep);

    Channel tricep("tricep", CH_2, AN_CA_2, 100, 250);
    channels.push_back(tricep);

    Channel forearm("forearm", CH_3, AN_CA_3, 100, 250);
    channels.push_back(forearm);

    Channel wrist("wrist", CH_4, AN_CA_4, 100, 250);
    channels.push_back(wrist);

    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", "COM11", channels, channels.size());

    // Initialize scheduler with the sync character and frequency of scheduler in hertz
    stim.create_scheduler(0xAA, 40);

    // Input which events will be added to the scheduler for updates
    stim.add_events(channels);

    std::thread viz_thread([&stim]() {
        Visualizer visualizer(&stim);
        visualizer.run();
    });

    // start sending stimulation to the board
    stim.begin();

    Timer timer(milliseconds(50), Timer::WaitMode::Hybrid);
    timer.set_acceptable_miss_rate(0.05);
    double t(0.0);

    while (!stop) {
        {
            // update the pulsewidth of each of the stimulation events
            stim.set_amp(bicep, 40 + int(10 * sin(t)));
            stim.set_amp(tricep, 30 + int(10 * sin(t)));
            stim.set_amp(forearm, 20 + int(10 * sin(t)));
            stim.set_amp(wrist, 10 + int(10 * sin(t)));

            // command the stimulation patterns to be sent to the stim board
            stim.update();
        }
        t = timer.wait().as_seconds();
    }

    // disable events, schedulers, boards, etc
    stim.disable();

    // viz_thread.join();

    return 0;
}
