#include "../ScopeTimer.hpp"

std::string ScopeTimer::scopeName;
std::chrono::high_resolution_clock::time_point ScopeTimer::startTimer;

void ScopeTimer::start(const char* timerName)
{
    scopeName = std::string(timerName);
    startTimer = std::chrono::high_resolution_clock::now();
}

void ScopeTimer::end()
{
    auto endTimer = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTimer - startTimer);
    std::cout << "Time taken for " << scopeName << ": " << std::to_string(elapsed.count() / 1e6) << "s" << std::endl;
}


