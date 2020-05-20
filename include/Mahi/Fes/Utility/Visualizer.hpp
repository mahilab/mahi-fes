#include <Mahi/Fes/Core/Stimulator.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <deque>
#include <mutex>
#include <thread>

namespace mahi {
namespace fes {
/// Visualizer for the FES stimulator. This opens a gui which shows the current pulsewidths
/// amplitudes from the stimulators. If enabled, it also allows for the gui to write to the
/// stimulator if it is enabled, but the c++ stimulator always overrides the visualizer.
class Visualizer : public mahi::gui::Application {
public:
    /// Visualizer constructor
    Visualizer(Stimulator *stimulator_);
    /// Visualizer destructor
    ~Visualizer();
    /// refreshes the gui
    void update() override;

private:
    ImGuiInputTextFlags          m_enabled_flags = 0;  // flags for showing whether user can read/write or just read
    int                          rt_axis = ImPlotAxisFlags_Default;// & ImAxisFlags_TickLabels & ImAxisFlags_GridLines;  // Flags for Realtime axis
    Stimulator *                 m_stimulator;    // stimulator pointer holding information to read
    std::vector<int>             m_amp;           // amplitudes from the stimulator
    std::vector<int>             m_pw;            // pulsewidths from the stimulator
    std::vector<int>             m_max_amp;       // maximum amplitudes from the stimulator
    std::vector<int>             m_max_pw;        // maximum pulsewidths from the stimulator
    size_t                       m_num_channels;  // number of total channels available
    std::deque<bool>             m_enabled = {false, false, false, false, false,
                                  false, false, false};  // whether each channel is enabled to control from the gui
    // ImGui::PlotInterface         m_plot_interface;                   // plot interface to handle plotting
    // std::vector<ImGui::PlotItem> m_items;                            // different items plotted on the graph
    mahi::util::Clock            m_elapse_clock;                     // total time elapsed since opening the gui
    std::vector<Channel>         m_channels;                         // vector of channels for the stimulator
    bool                         m_open = true;                      // whether the application is open or not
    std::mutex                   m_mtx;                              // mutex to handle simultaneous reading/writing
    std::vector<ImVec4>          m_color = {
        ImVec4(0.0f / 255.0f, 218.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(20.0f / 255.0f, 220.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(255.0f / 255.0f, 120.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(150.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(0.0f / 255.0f, 30.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f),
        ImVec4(126.0f / 255.0f, 126.0f / 255.0f, 126.0f / 255.0f, 255.0f / 255.0f),
    };  // vector of predefined color options

    struct ScrollingData {
        int MaxSize = 1000;
        int Offset  = 0;
        ImVector<ImVec2> Data;
        ScrollingData() { Data.reserve(MaxSize); }
        void AddPoint(float x, float y) {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x,y));
            else {
                Data[Offset] = ImVec2(x,y);
                Offset =  (Offset + 1) % MaxSize;
            }
        }
    };

    std::vector<ScrollingData> channel_data_amp;
    std::vector<ScrollingData> channel_data_pw;
};
}  // namespace fes
}  // namespace mahi
