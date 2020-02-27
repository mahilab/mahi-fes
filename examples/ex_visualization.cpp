// #define MAHI_GUI_NO_CONSOLE
#include "mahi/gui.hpp"
#include <FES/Core/Stimulator.hpp>
#include <FES/Core/Channel.hpp>
#include <FES/Core/Event.hpp>
#include <FES/Core/Scheduler.hpp>
#include <FES/Utility/Utility.hpp>
#include <thread>
#include <mutex>
#include <deque>
#include <iostream>
#include <MEL/Core/Time.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Math.hpp>
#include <thread>
#include <atomic>
#include <FES/Utility/Visualizer.hpp>


using namespace mahi::gui;
using namespace fes;

int main(int argc, char const *argv[])
{
    std::cout << "here" << std::endl;
    // create channels of interest
    std::vector<Channel> channels;

    Channel bicep  ("Bicep",   CH_1, 100, 250);
    channels.push_back(bicep);

    Channel tricep ("Tricep",  CH_2, 100, 250);
    channels.push_back(tricep);

    Channel forearm("Forearm Pronation", CH_3, 100, 250);
    channels.push_back(forearm);
    
    Channel wrist  ("Wrist Flexion",   CH_4, 100, 250);
    channels.push_back(wrist);
    
    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", "COM5", channels, channels.size());

    std::mutex mtx;
    mel::Timer control_timer(mel::milliseconds(1));
    mel::Time current_t = mel::Time::Zero;

    std::thread viz_thread([&stim](){
        Visualizer visualizer(&stim);
    });
    
    control_timer.restart();
    while (true){
        {
            std::lock_guard<std::mutex> lock(mtx);
            stim.amplitudes[0] = int(50+50*mel::sin(current_t.as_seconds()));
            stim.amplitudes[1] = int(50+50*mel::cos(current_t.as_seconds()));
        }
        current_t = control_timer.wait();
    }

    viz_thread.join();
    
    return 0;
}

