#include <FES/Utility/VirtualStim.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Core/Timer.hpp>

using namespace mel;
using namespace fes;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main(){
    register_ctrl_handler(handler);

    VirtualStim vstim("COM10");

    Timer poll_timer(milliseconds(1), Timer::WaitMode::Hybrid);

    while(!stop){
        vstim.poll();
    }
}