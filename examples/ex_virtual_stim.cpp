#include <Mahi/Fes/Utility/VirtualStim.hpp>
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

    VirtualStim vstim("COM10");
    vstim.begin();
    return 0;
}