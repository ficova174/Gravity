#include <chrono>
#include <iostream>

class Timer {
public:
    void reset() {
        start = std::chrono::high_resolution_clock::now();
    }

    void elapsed() {
        auto now = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration<double>(now - start).count() << '\n';
    }

private:
    std::chrono::high_resolution_clock::time_point start;
};
