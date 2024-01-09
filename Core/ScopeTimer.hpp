#pragma once

#include <string>
#include <chrono>
#include <iostream>

class ScopeTimer
{
    private:
        static std::string scopeName;
        static std::chrono::high_resolution_clock::time_point startTimer;
    
    public:
        static void start(const char* timerName);
        static void end();
};
