#define MAHI_GUI_NO_CONSOLE
#include "mahi/gui.hpp"
#include <thread>
#include <mutex>

using namespace mahi::gui;

class StimViz : public Application {
public:
    StimViz() : Application(1000,800,"Stimulator Visualization"){
        ImGui::StyleColorsLight();
        hideWindow();
    }

    void update(){
        ImGui::Begin("FES Stim", &open);
        ImGui::BeginGroup();
            ImGui::Text("Stimulator Visualization");
            ImGui::Separator();
            
            for (size_t i = 0; i < 8; i++){
                static ImVec4 color = ImVec4(114.0f/255.0f, 144.0f/255.0f, 154.0f/255.0f, 200.0f/255.0f);
                ImGui::ColorEdit4("MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine(); 
                ImGui::Text("Channel 1: Bicep");
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
                ImGui::BeginGroup();
                    ImGui::LabelText("##Max Amp","Max Amp");
                    ImGui::SameLine();
                    ImGui::InputInt("maxamp", &max_amp[i]);
                    ImGui::LabelText("##Max PW","Max PW");
                    ImGui::SameLine();
                    ImGui::InputInt("maxpw", &max_pw[i]);
                ImGui::EndGroup();
                ImGui::PopItemWidth();
                ImGui::SameLine();
                ImGui::BeginGroup();
                    ImGui::Checkbox("Enable", &enabled[i]);
                    ImGui::Checkbox("Plot", &plot[i]);
                ImGui::EndGroup();
                ImGui::Separator();
            }
            
            
        ImGui::EndGroup();
        ImGui::End();

        if (!open){
            quit();
        }
    }
    const int len = 8;
    std::vector<int> amp = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> pw = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> max_amp  = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<int> max_pw  = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<bool> plot = {false, false, false, false, false, false, false, false};
    std::vector<bool> enabled = {false, false, false, false, false, false, false, false};
    bool open = true;
};

int main(int argc, char const *argv[])
{
    StimViz visualizer;
    visualizer.run();
    return 0;
}

