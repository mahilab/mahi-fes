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

int main(int argc, char const *argv[]) {
    // create channels of interest
    std::vector<Channel> channels;

    Channel bicep("Bicep", CH_1, AN_CA_1, 100, 250);
    channels.push_back(bicep);

    Channel tricep("Tricep", CH_2, AN_CA_2, 100, 250);
    channels.push_back(tricep);

    Channel forearm("Forearm Pronation", CH_3, AN_CA_3, 100, 250);
    channels.push_back(forearm);

    Channel wrist("Wrist Flexion", CH_4, AN_CA_4, 100, 250);
    channels.push_back(wrist);

    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", "COM9", channels, channels.size());

    // Initialize scheduler with the sync character and frequency of scheduler in hertz
    stim.create_scheduler(0xAA, 100);

    // Input which events will be added to the scheduler for updates
    stim.add_events(channels);

    std::mutex mtx;
    Timer      control_timer(milliseconds(50));
    Time       current_t = Time::Zero;

    stim.begin();

    std::thread viz_thread([&stim]() {
        Visualizer visualizer(&stim);
        visualizer.run();
    });

    control_timer.restart();

    while (true) {
        {
            // std::lock_guard<std::mutex> lock(mtx);
            // stim.set_amp(bicep,int(50+50*mel::sin(current_t.as_seconds())));
            // stim.set_amp(tricep,int(50+50*mel::cos(current_t.as_seconds())));

            stim.update();
        }
        current_t = control_timer.wait();
    }

    viz_thread.join();

    return 0;
}
