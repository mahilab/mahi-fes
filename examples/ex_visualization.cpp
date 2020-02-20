#define MAHI_GUI_NO_CONSOLE
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

using namespace mahi::gui;
using namespace fes;

class StimViz : public Application {
public:
    StimViz(Stimulator *stimulator_) : Application(1300,800,"Stimulator Visualization")
    {
        // ImGui::DisableViewports();
        stimulator = stimulator_;
        amp = stimulator->amplitudes;
        pw = stimulator->pulsewidths;
        max_amp = stimulator->max_amplitudes;
        max_pw = stimulator->max_pulsewidths;
        num_channels = stimulator->num_events;
        // stop = stop_;
        ImGui::StyleColorsLight();
        hideWindow();
        plot_interface.xAxis.minimum = 0;   
        plot_interface.xAxis.maximum = 10;
        plot_interface.yAxis.minimum = -1;   
        plot_interface.yAxis.maximum = float(*max_element(max_amp.begin(), max_amp.end()));
        plot_interface.xAxis.showLabels = true;
        // plot_interface.
        items.resize(num_channels);
        for (auto i = 0; i < num_channels; i++){
            items[i].color = Grays::Gray50;
            items[i].data.reserve(20000);
            items[i].size = 3;
        }
        elapse_clock.restart();
    }

    ~StimViz(){
        stop = true;
    }

    void roll_point(ImGui::PlotItem& item, mel::Time t, int pos) {
        float tmod = float(fmod(t.as_seconds(), 10));
        if (!item.data.empty() && tmod < item.data.back().x)
            item.data.clear();
        item.data.push_back({tmod, float(pos)});
    }

    void update(){
        {
            std::lock_guard<std::mutex> lock(mtx);
            amp = stimulator->amplitudes;
            pw = stimulator->pulsewidths;
            max_amp = stimulator->max_amplitudes;
            max_pw = stimulator->max_pulsewidths;
        }

        ImGui::Begin("FES Stim", &open);
        ImGui::BeginGroup();
            ImGui::Text("Stimulator Visualization");
            ImGui::Separator();
            
            for (size_t i = 0; i < num_channels; i++){
                ImGui::ColorEdit4(("##Color"+ std::to_string(i)).c_str(), (float*)&color[i], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine(); 
                ImGui::Text(("Channel " + std::to_string(i+1) + ": " + stimulator->channel_names[i]).c_str());
                ImGui::Separator();
                ImGui::PushItemWidth(70);

                ImGui::BeginGroup();
                    ImGui::LabelText("##Amplitude", "Amplitude");
                    ImGui::InputInt(("##amp" + std::to_string(i)).c_str(), &amp[i]);
                ImGui::EndGroup();
                ImGui::SameLine(100);
                ImGui::BeginGroup();
                    ImGui::LabelText("##PulseWidth","PulseWidth");
                    ImGui::InputInt(("##pw" + std::to_string(i)).c_str(), &pw[i]);
                ImGui::EndGroup();

                ImGui::SameLine(200);
                ImGui::PopItemWidth();
                ImGui::BeginGroup();
                    ImGui::PushItemWidth(50);
                    ImGui::LabelText("##Max Amp","Max Amp");
                    ImGui::SameLine();
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(75);
                    ImGui::InputInt(("##maxamp" + std::to_string(i)).c_str(), &max_amp[i]);
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(50);
                    ImGui::LabelText("##Max PW","Max PW");
                    ImGui::SameLine();
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(75);
                    ImGui::InputInt(("##maxpw" + std::to_string(i)).c_str(), &max_pw[i]);
                ImGui::EndGroup();
                ImGui::PopItemWidth();
                
                ImGui::SameLine(350);
                ImGui::PopItemWidth();
                ImGui::PushItemWidth(180);
                ImGui::BeginGroup();
                    ImGui::PushItemWidth(45);
                    ImGui::LabelText("##Enable", "Enable");
                    ImGui::SameLine();
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(75);
                    ImGui::Checkbox(("##Enable" + std::to_string(i)).c_str(), &enabled[i]);
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(45);
                    ImGui::LabelText("##Plot", "Plot");
                    ImGui::SameLine();
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(75);
                    ImGui::Checkbox(("##Plot" + std::to_string(i)).c_str(), &items[i].show);
                ImGui::EndGroup();
                ImGui::PopItemWidth();
                ImGui::Separator();
                ImGui::Separator();
            }

        for (auto i = 0; i < num_channels; i++){
            roll_point(items[i], elapse_clock.get_elapsed_time(), amp[i]);
        }
        
        ImGui::EndGroup();
        ImGui::SameLine();
        std::vector<ImGui::PlotItem> plot_items;
        for (auto i = 0; i < num_channels; i++){
            items[i].color = color[i];
        }
        
        ImGui::Plot("Position", plot_interface, items);
        ImGui::End();

        {
            std::lock_guard<std::mutex> lock(mtx);
            stimulator->amplitudes = amp;
            stimulator->pulsewidths = pw;
            stimulator->max_amplitudes = max_amp;
            stimulator->max_pulsewidths = max_pw;
        }

        if (!open || stop){
            quit();
        }
        
    }

    // std::vector<std::string> channel_names = {"Bicep", "Tricep", "Forearm Pronation", "Forearm Supination", "Wrist Flexion", "Wrist Extension", "Wrist Radial Dev", "Wrist Ulnar Dev"};

    Stimulator *stimulator;
    std::vector<int> amp;
    std::vector<int> pw;
    std::vector<int> max_amp;
    std::vector<int> max_pw;
    size_t num_channels;
    std::deque<bool> enabled = {false, false, false, false, false, false, false, false};
    ImGui::PlotInterface plot_interface;
    std::vector<ImGui::PlotItem> items;
    mel::Clock elapse_clock;
    bool open = true;
    std::atomic_bool stop = false;
    std::mutex mtx;
    std::vector<ImVec4> color = {ImVec4(0.0f/255.0f,   218.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f),
                                 ImVec4(20.0f/255.0f,  220.0f/255.0f, 0.0f/255.0f,   255.0f/255.0f),
                                 ImVec4(255.0f/255.0f, 120.0f/255.0f, 0.0f/255.0f,   255.0f/255.0f),
                                 ImVec4(255.0f/255.0f, 0.0f/255.0f,   0.0f/255.0f,   255.0f/255.0f),
                                 ImVec4(150.0f/255.0f, 0.0f/255.0f,   255.0f/255.0f, 255.0f/255.0f),
                                 ImVec4(0.0f/255.0f,   30.0f/255.0f,  255.0f/255.0f, 255.0f/255.0f),
                                 ImVec4(255.0f/255.0f, 0.0f/255.0f,   255.0f/255.0f, 255.0f/255.0f),
                                 ImVec4(126.0f/255.0f, 126.0f/255.0f, 126.0f/255.0f, 255.0f/255.0f),};
};

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
        StimViz visualizer(&stim);
        visualizer.run();
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

