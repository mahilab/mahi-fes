#include <Mahi/Fes.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::util;
using namespace mahi::fes;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool      handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main() {
    register_ctrl_handler(handler);

    // This runs a virtual stimulator listener that waits for information from the comport specified. The way I use this
    // is to run a comport emulator (http://com0com.sourceforge.net) which emulates two comports. Set the comport in the
    // Virtual Stim object below to be one comport, and run a separate code (eg. test_stim) with the comport of the 
    // stimulator set to the other comport of the emulator. The messages sent should appear in the virtual stim gui below
    VirtualStim vstim("COM10");
    vstim.run();
    return 0;
}