#pragma once
#include <memory>

namespace AudioSystem {
    class Manager;
    using ManagerPtr = std::unique_ptr<Manager>; 
}
