#include "MyClass.hpp"
#include <Mahi/Util.hpp>

using namespace mahi::util;

void MyClass::hello_world() {
    std::string response = "World";
    print("Hello, {}!", response);
}
