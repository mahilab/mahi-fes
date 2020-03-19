// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Nathan Dunkelberger (nbd2@rice.edu)

#include <Mahi/Fes/Core/Stimulator.hpp>
#include <Mahi/Fes/Utility/Visualizer.hpp>
#include <Mahi/Gui.hpp>

using namespace mahi::gui;
using namespace mahi::util;

namespace mahi {
namespace fes {

Visualizer::Visualizer(Stimulator* stimulator_) :
    Application(),
    stimulator(stimulator_),
    amp(stimulator->amplitudes),
    pw(stimulator->pulsewidths),
    max_amp(stimulator->max_amplitudes),
    max_pw(stimulator->max_pulsewidths),
    num_channels(stimulator->num_events),
    channels(stimulator->get_channels()) {
    // initialize theme
    ImGui::StyleColorsLight();
    hide_window();

    // Add plotting parameters necessary
    plot_interface.x_axis.minimum          = 0;
    plot_interface.x_axis.maximum          = 10;
    plot_interface.y_axis.minimum          = -1;
    plot_interface.y_axis.maximum          = float(*max_element(max_amp.begin(), max_amp.end()));
    plot_interface.x_axis.show_tick_labels = true;

    items.resize(num_channels);
    for (auto i = 0; i < num_channels; i++) {
        items[i].color = Grays::Gray50;
        items[i].data.reserve(20000);
        items[i].size = 3;
    }
    elapse_clock.restart();
}

Visualizer::~Visualizer() {}

void Visualizer::update() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        amp     = stimulator->amplitudes;
        pw      = stimulator->pulsewidths;
        max_amp = stimulator->max_amplitudes;
        max_pw  = stimulator->max_pulsewidths;
    }

    ImGui::Begin("FES Stim", &open);
    ImGui::BeginGroup();
    ImGui::Text("Stimulator Visualization");
    ImGui::Separator();

    for (size_t i = 0; i < num_channels; i++) {
        ImGui::ColorEdit4(("##Color" + std::to_string(i)).c_str(), (float*)&color[i],
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Text(
            ("Channel " + std::to_string(i + 1) + ": " + channels[i].get_channel_name()).c_str());
        ImGui::Separator();
        ImGui::PushItemWidth(70);

        if (!enabled[i]) {
            enabled_flags = ImGuiInputTextFlags_ReadOnly;
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        } else {
            enabled_flags = 0;
        }
        ImGui::BeginGroup();
        ImGui::LabelText("##Amplitude", "Amplitude");
        ImGui::InputInt(("##amp" + std::to_string(i)).c_str(), &amp[i], 1, 100, enabled_flags);

        ImGui::EndGroup();
        ImGui::SameLine(100);
        ImGui::BeginGroup();
        ImGui::LabelText("##PulseWidth", "PulseWidth");
        ImGui::InputInt(("##pw" + std::to_string(i)).c_str(), &pw[i], 1, 100, enabled_flags);
        ImGui::EndGroup();

        ImGui::SameLine(200);
        ImGui::PopItemWidth();
        ImGui::BeginGroup();
        ImGui::PushItemWidth(50);
        ImGui::LabelText("##Max Amp", "Max Amp");
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(75);
        ImGui::InputInt(("##maxamp" + std::to_string(i)).c_str(), &max_amp[i], 1, 100,
                        enabled_flags);
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(50);
        ImGui::LabelText("##Max PW", "Max PW");
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(75);
        ImGui::InputInt(("##maxpw" + std::to_string(i)).c_str(), &max_pw[i], 1, 100, enabled_flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();

        if (!enabled[i])
            ImGui::PopStyleVar();

        ImGui::SameLine(350);
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

    for (auto i = 0; i < num_channels; i++) {
        ImGui::PlotItemBufferPoint(items[i], static_cast<float>(time().as_seconds()),
                                   static_cast<float>(amp[i]), 20000);
    }
    plot_interface.x_axis.minimum = (float)elapse_clock.get_elapsed_time().as_seconds() - 10;
    plot_interface.x_axis.maximum = (float)elapse_clock.get_elapsed_time().as_seconds();

    ImGui::EndGroup();
    ImGui::SameLine();
    std::vector<ImGui::PlotItem> plot_items;
    for (auto i = 0; i < num_channels; i++) {
        items[i].color = color[i];
    }

    ImGui::Plot("Position", plot_interface, items);
    ImGui::End();

    for (size_t i = 0; i < num_channels; i++) {
        amp[i] = (amp[i] < 0) ? 0 : amp[i];
        amp[i] = (amp[i] < max_amp[i]) ? amp[i] : max_amp[i];

        pw[i] = (pw[i] < 0) ? 0 : pw[i];
        pw[i] = (pw[i] < max_pw[i]) ? pw[i] : max_pw[i];
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < num_channels; i++) {
            if (enabled[i]) {
                stimulator->write_amp(channels[i], amp[i]);
                stimulator->write_pw(channels[i], pw[i]);
                stimulator->update_max_amp(channels[i], max_amp[i]);
                stimulator->update_max_pw(channels[i], max_pw[i]);
                stimulator->amplitudes[i]      = amp[i];
                stimulator->pulsewidths[i]     = pw[i];
                stimulator->max_amplitudes[i]  = max_amp[i];
                stimulator->max_pulsewidths[i] = max_pw[i];
            }
        }
    }

    if (!open) {
        quit();
    }
}
}  // namespace fes
}  // namespace mahi