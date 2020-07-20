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

    // // create new channel for brachioradialis (Channel 5, Anode Cathode Pair 1 - because it is a new board, max amp 100 mA, max pw 250 us)
    // Channel fcr("Flexor Carpi Radialis", CH_5, AN_CA_1, 100, 250);
    // channels.push_back(fcr);

    // Create stim board with a name, channels to add, and comports used
    Stimulator stim("UECU Board", channels, "COM11", "NONE", false);

    // Initialize scheduler with the sync character and frequency of scheduler in hertz. If using all 4 channels on a board,
    // this must be < 67 Hz because of how the channels are spaced out. The scheduler must have a period that can handle
    // 5ms spacing for each of the channels, ie. channels update at 0ms, 5ms, 10ms, 15ms. therefore, 1000/66 = 15.15, meaning
    // it updates every 15.15 seconds 
    stim.create_scheduler(0xAA, 40);

    // Input which events will be added to the scheduler for updates (usually, this will just be all events)
    stim.add_events(channels);

    // start the visualization thread to run the gui. This is optional, but allows the stimulators to be updated through
    // the gui rather than in code. The code will overwrite the gui, so if gui control is desired, remove updates in the
    // while loop below
    std::thread viz_thread([&stim]() {
        Visualizer visualizer(&stim);
        visualizer.run();
    });

    Clock test_clock;

    // start sending stimulation to the board
    stim.begin();

    // set timer for our control loop
    Timer timer(milliseconds(50), Timer::WaitMode::Hybrid);
    timer.set_acceptable_miss_rate(0.05);
    
    // variable to keep track of our current time
    double t(0.0);

    enable_realtime();

    while (!stop) {
        {
            // update the pulsewidth of each of the stimulation events
            stim.set_amp(bicep, 60);
            stim.write_pw(bicep, 10 + int(10 * sin(t)));

            // command the stimulation patterns to be sent to the stim board.
            // This is required whether using the gui or updating in code.
            test_clock.restart();
            stim.update();
            print_var(test_clock.get_elapsed_time().as_microseconds());
        }
        // wait for the loop to end
        t = timer.wait().as_seconds();
    }

    // disable events, schedulers, boards, etc
    stim.disable();

    // join the visualizer thread *only if it was enabled earlier
    viz_thread.join();

    disable_realtime();

    return 0;
}
