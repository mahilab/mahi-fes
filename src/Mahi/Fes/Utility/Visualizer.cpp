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
    m_stimulator(stimulator_),
    m_amp(m_stimulator->amplitudes),
    m_pw(m_stimulator->pulsewidths),
    m_max_amp(m_stimulator->max_amplitudes),
    m_max_pw(m_stimulator->max_pulsewidths),
    m_num_channels(m_stimulator->num_events),
    m_channels(m_stimulator->get_channels()) {
    // initialize theme
    ImGui::StyleColorsLight();
    hide_window();

    // Add plotting parameters necessary
    m_plot_interface.x_axis.minimum          = 0;
    m_plot_interface.x_axis.maximum          = 10;
    m_plot_interface.y_axis.minimum          = -1;
    m_plot_interface.y_axis.maximum          = float(*max_element(m_max_amp.begin(), m_max_amp.end()));
    m_plot_interface.x_axis.show_tick_labels = true;

    m_items.resize(m_num_channels);
    for (auto i = 0; i < m_num_channels; i++) {
        m_items[i].color = Grays::Gray50;
        m_items[i].data.reserve(20000);
        m_items[i].size = 3;
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
        ImGui::Checkbox(("##Plot" + std::to_string(i)).c_str(), &m_items[i].show);
        ImGui::EndGroup();
        ImGui::PopItemWidth();
        ImGui::Separator();
        ImGui::Separator();
    }

    for (auto i = 0; i < m_num_channels; i++) {
        ImGui::PlotItemBufferPoint(m_items[i], static_cast<float>(time().as_seconds()),
                                   static_cast<float>(m_amp[i]), 20000);
    }
    m_plot_interface.x_axis.minimum = (float)m_elapse_clock.get_elapsed_time().as_seconds() - 10;
    m_plot_interface.x_axis.maximum = (float)m_elapse_clock.get_elapsed_time().as_seconds();

    ImGui::EndGroup();
    ImGui::SameLine();
    std::vector<ImGui::PlotItem> plot_items;
    for (auto i = 0; i < m_num_channels; i++) {
        m_items[i].color = m_color[i];
    }

    ImGui::Plot("Position", m_plot_interface, m_items);
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