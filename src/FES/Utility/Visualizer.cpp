#include <FES/Utility/Visualizer.hpp>
#include <mahi/gui.hpp>
#include <FES/Core/Stimulator.hpp>

using namespace fes;
using namespace mahi::gui;

Visualizer::Visualizer(Stimulator *stimulator_) : Application(1300,800,"Stimulator Visualization"),
    stimulator(stimulator_),
    amp(stimulator->amplitudes),
    pw(stimulator->pulsewidths),
    max_amp(stimulator->max_amplitudes),
    max_pw(stimulator->max_pulsewidths),
    num_channels(stimulator->num_events),
    channels(stimulator->get_channels())
    {
        // initialize theme
        ImGui::StyleColorsLight();
        hideWindow();
        
        // Add plotting parameters necessary
        plot_interface.xAxis.minimum = 0;   
        plot_interface.xAxis.maximum = 10;
        plot_interface.yAxis.minimum = -1;   
        plot_interface.yAxis.maximum = float(*max_element(max_amp.begin(), max_amp.end()));
        plot_interface.xAxis.showLabels = true;

        items.resize(num_channels);
        for (auto i = 0; i < num_channels; i++){
            items[i].color = Grays::Gray50;
            items[i].data.reserve(20000);
            items[i].size = 3;
        }
        elapse_clock.restart();

        run();
    }

Visualizer::~Visualizer(){
    viz_thread.join();
}

void Visualizer::roll_point(ImGui::PlotItem& item, mel::Time t, int pos) {
    float tmod = float(fmod(t.as_seconds(), 10));
    if (!item.data.empty() && tmod < item.data.back().x)
        item.data.clear();
    item.data.push_back({tmod, float(pos)});
}

void Visualizer::update(){
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
            ImGui::Text(("Channel " + std::to_string(i+1) + ": " + channels[i].get_channel_name()).c_str());
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
        // WORK ON THIS
        stimulator->write_amps(channels,amp);
        stimulator->write_pws(channels,pw);
        stimulator->max_amplitudes = max_amp;
        stimulator->max_pulsewidths = max_pw;
    }

    if (!open){
        quit();
    }
        
}


