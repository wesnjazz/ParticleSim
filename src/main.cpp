#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <chrono> // Standard for measuring time (C++11)

// [PERFORMANCE] Array of Structures (AOS) Pattern
// For 5,000 particles, this is okay. For 100,000+, accessing x, then y, then vx
// causes "cache thrashing" because the CPU has to fetch the whole struct just to update 'x'.
// A "Structure of Arrays" (SOA) layout (vector<float> x, vector<float> y) is often faster.
struct Particle {
    float x, y;
    float vx, vy;
};

// [SAFETY] Buffer Overflow Risk
// There is no guarantee that 'data' actually has 'count' elements.
// If I accidentally pass count = 5001, this function will overwrite memory
// belonging to other variables, leading to a crash or security exploit.
void update_particles(Particle* data, int count, float dt) {
    for (int i = 0; i < count; ++i) {
        data[i].x += data[i].vx * dt;
        data[i].y += data[i].vy * dt;

        if (data[i].x < 0 || data[i].x > 500) data[i].vx *= -1;
        if (data[i].y < 0 || data[i].y > 500) data[i].vy *= -1;
    }
}

int main()
{
    const int NUM_PARTICLES = 5000;

    // [SAFETY] Memory Leak Risk (The "Naked New")
    // We are manually asking the OS for memory. C++ does not track this for us.
    // If we throw an exception or return before line "delete", this memory is lost forever (Leak).
    Particle* particles = new Particle[NUM_PARTICLES];

    for (int i = 0; i < NUM_PARTICLES; ++i) {
        // [PERFORMANCE] Slow Random Number Generator
        // 'rand()' is notoriously slow and low-quality.
        // It uses a global lock which kills performance in multithreaded apps.
        particles[i].x = rand() % 500;
        particles[i].y = rand() % 500;
        particles[i].vx = (rand() % 10 - 5) * 5.0f; // Random velocity -5 to 5
        particles[i].vy = (rand() % 10 - 5) * 5.0f; // Random velocity -5 to 5
    }

    // --- METRICS VARIABLES ---
    long long total_latency_us = 0;
    long long frame_count = 0;
    // -------------------------

    std::cout << "Starting Legacy Simulation (C++ 98 Style)..." << std::endl;
    std::cout << "Press ESC to quit." << std::endl;

    while (true) {
        auto start_time = std::chrono::high_resolution_clock::now();

        // [SAFETY] Disconnected Size
        // We have to manually ensure 'NUM_PARTICLES' matches the array size.
        // If we changed the array allocation above but forgot to change this constant, crash!
        update_particles(particles, NUM_PARTICLES, 0.1f);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        long long current_latency = duration.count();

        total_latency_us += current_latency;
        frame_count++;
        double average_latency = (double)total_latency_us / frame_count;

        // [PERFORMANCE] Unnecessary Allocation
        // 'cv::Mat::zeros' allocates NEW memory on the heap every single frame (60 times a second).
        // It is better to create 'cv::Mat frame' ONCE outside the loop and just use 'frame.setTo(0)'.
        cv::Mat frame = cv::Mat::zeros(500, 500, CV_8UC3);

        for (int i = 0; i < NUM_PARTICLES; ++i) {
            cv::Scalar color = (i < NUM_PARTICLES/2) ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 255);
            cv::circle(frame, cv::Point(particles[i].x, particles[i].y), 2, color, -1);
        }

        std::string instant_text = "Instant: " + std::to_string(current_latency) + " us";
        std::string avg_text     = "Average: " + std::to_string((int)average_latency) + " us";

        cv::putText(frame, instant_text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, avg_text,     cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

        cv::imshow("Legacy Sim", frame);

        // [PERFORMANCE] Blocking the Thread
        // 'waitKey(16)' halts the ENTIRE program (including physics) for 16ms to wait for input.
        // In a real simulation, we want physics to run as fast as possible in the background
        // while the GUI waits separately (Multithreading needed).
        if (cv::waitKey(16) == 27) break;   // 27 is ESC
    }

    // [SAFETY] Dangling Pointer Risk
    // After this line, 'particles' still holds the address, but the memory is gone.
    // If someone tries to access 'particles[0]' after this line, the program enters Undefined Behavior.
    delete[] particles;

    std::cout << "Simulation finished. Memory cleaned up." << std::endl;

    return 0;
}