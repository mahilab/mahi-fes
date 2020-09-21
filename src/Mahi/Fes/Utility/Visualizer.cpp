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
    Application(500,500,"Visualizer"),
    m_stimulator(stimulator_),
    m_amp(m_stimulator->amplitudes),
    m_pw(m_stimulator->pulsewidths),
    m_max_amp(m_stimulator->max_amplitudes),
    m_max_pw(m_stimulator->max_pulsewidths),
    m_num_channels(m_stimulator->num_events),
    m_channels(m_stimulator->get_channels()) {
    // initialize theme
    ImGui::StyleColorsLight();

    for (auto i = 0; i < m_num_channels; i++) {
        ScrollingData new_scroll_data_amp;
        ScrollingData new_scroll_data_pw;
        channel_data_amp.push_back(new_scroll_data_amp);
        channel_data_pw.push_back(new_scroll_data_pw);
    }
    m_elapse_clock.restart();
}

Visualizer::~Visualizer() {}

void Visualizer::update() {
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_amp     = m_stimulator->amplitudes;
        m_pw      = m_stimulator->pulsewidths;
        m_max_amp = m_stimulator->max_amplitudes;
        m_max_pw  = m_stimulator->max_pulsewidths;
    }

    ImGui::Begin("FES Stim", &m_open);
    ImGui::BeginGroup();
    ImGui::Text("Stimulator Visualization");
    ImGui::Separator();

    for (size_t i = 0; i < m_num_channels; i++) {
        ImGui::ColorEdit4(("##Color" + std::to_string(i)).c_str(), (float*)&m_color[i],
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Text(
            ("Channel " + std::to_string(i + 1) + ": " + m_channels[i].get_channel_name()).c_str());
        ImGui::Separator();
        ImGui::PushItemWidth(70);

        if (!m_enabled[i]) {
            m_enabled_flags = ImGuiInputTextFlags_ReadOnly;
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        } else {
            m_enabled_flags = 0;
        }
        ImGui::BeginGroup();
        ImGui::LabelText("##Amplitude", "Amplitude");
        ImGui::InputInt(("##amp" + std::to_string(i)).c_str(), &m_amp[i], 1, 100, m_enabled_flags);

        ImGui::EndGroup();
        ImGui::SameLine(100);
        ImGui::BeginGroup();
        ImGui::LabelText("##PulseWidth", "PulseWidth");
        ImGui::InputInt(("##pw" + std::to_string(i)).c_str(), &m_pw[i], 1, 100, m_enabled_flags);
        ImGui::EndGroup();

        ImGui::SameLine(200);
        ImGui::PopItemWidth();
        ImGui::BeginGroup();
        ImGui::PushItemWidth(50);
        ImGui::LabelText("##Max Amp", "Max Amp");
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(75);
        ImGui::InputInt(("##maxamp" + std::to_string(i)).c_str(), &m_max_amp[i], 1, 100,
                        m_enabled_flags);
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(50);
        ImGui::LabelText("##Max PW", "Max PW");
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(75);
        ImGui::InputInt(("##maxpw" + std::to_string(i)).c_str(), &m_max_pw[i], 1, 100, m_enabled_flags);
        ImGui::PopItemWidth();
        ImGui::EndGroup();

        if (!m_enabled[i])
            ImGui::PopStyleVar();

        ImGui::SameLine(350);
        ImGui::PushItemWidth(180);
        ImGui::BeginGroup();
        ImGui::PushItemWidth(45);
        ImGui::LabelText("##Enable", "Enable");
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(75);
        ImGui::Checkbox(("##Enable" + std::to_string(i)).c_str(), &m_enabled[i]);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(45);
        ImGui::LabelText("##Plot", "Plot");
        ImGui::SameLine();
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(75);
        ImGui::EndGroup();
        ImGui::PopItemWidth();
        ImGui::Separator();
        ImGui::Separator();
    }
   
    ImGui::EndGroup();
    ImGui::SameLine();

    float t = (float)m_elapse_clock.get_elapsed_time().as_seconds();
    for (auto i = 0; i < m_num_channels; i++) {
        channel_data_amp[i].AddPoint(t,m_amp[i]);
        channel_data_pw[i].AddPoint(t,m_pw[i]);
    }
    
    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
    
    ImPlot::SetNextPlotLimitsX(t - 10, t, ImGuiCond_Always);
    ImPlot::SetNextPlotLimitsY(0,100);
    if(ImPlot::BeginPlot("##FES Plot", "Time (s)", "Amplitude(mA)", {-1,-1}, 0, rt_axis, rt_axis)){
            for (size_t i = 0; i < m_num_channels; i++) {
                ImPlot::PushStyleColor(ImPlotCol_Line, m_color[i]);
                // ImPlot::PlotLine(("Channel "+std::to_string(i) + " Amplitude").c_str(), &channel_data_amp[i].Data[0].x, &channel_data_amp[i].Data[0].y, channel_data_amp[i].Data.size(), channel_data_amp[i].Offset, 2 * sizeof(float));
                ImPlot::PlotLine(("Channel "+std::to_string(i+1) + " Pulsewidth").c_str(), &channel_data_pw[i].Data[0].x, &channel_data_pw[i].Data[0].y, channel_data_pw[i].Data.size(), channel_data_pw[i].Offset, 2 * sizeof(float));
                ImPlot::PopStyleColor();
            }
            ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();

    ImGui::End();

    for (size_t i = 0; i < m_num_channels; i++) {
        m_amp[i] = (m_amp[i] < 0) ? 0 : m_amp[i];
        m_amp[i] = (m_amp[i] < m_max_amp[i]) ? m_amp[i] : m_max_amp[i];

        m_pw[i] = (m_pw[i] < 0) ? 0 : m_pw[i];
        m_pw[i] = (m_pw[i] < m_max_pw[i]) ? m_pw[i] : m_max_pw[i];
    }

    {
        std::lock_guard<std::mutex> lock(m_mtx);
        for (size_t i = 0; i < m_num_channels; i++) {
            if (m_enabled[i]) {
                m_stimulator->set_amp(m_channels[i], m_amp[i]);
                m_stimulator->write_pw(m_channels[i], m_pw[i]);
                m_stimulator->update_max_amp(m_channels[i], m_max_amp[i]);
                m_stimulator->update_max_pw(m_channels[i], m_max_pw[i]);
                m_stimulator->amplitudes[i]      = m_amp[i];
                m_stimulator->pulsewidths[i]     = m_pw[i];
                m_stimulator->max_amplitudes[i]  = m_max_amp[i];
                m_stimulator->max_pulsewidths[i] = m_max_pw[i];
            }
        }
    }

    if (!m_open) {
        quit();
    }
}
}  // namespace fes
}  // namespace mahi