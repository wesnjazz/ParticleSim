#include <iostream>
#include <cstdlib>
#include <vector>   // [UPDATE] Added standard container library
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

// [UPDATE] Function Signature Change
// Old: void update_particles(Particle* data, int count, float dt)
// New: We pass std::vector by Reference (&).
// Why Reference? If we passed by value "vector<Particle> data", C++ would COPY
// all 5,000 particles every frame. '&' lets us modify the original memory.
// Why Safe? The vector carries its own size(). We can't accidentally read past the end.
// [WARNING] This is "Stiff": This function now REFUSES to accept raw arrays.
void update_particles(std::vector<Particle>& data, float dt) {
    // [UPDATE] Range-Based For Loop (C++11 Feature)
    // Instead of: for(int i=0; i < count; ++i)
    // We say: "For every particle 'p' in 'data'..."
    // This makes "Off-by-one" index errors impossible.
    for (auto& p : data) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        if (p.x < 0 || p.x > 500) p.vx *= -1;
        if (p.y < 0 || p.y > 500) p.vy *= -1;
    }
}

int main()
{
    const int NUM_PARTICLES = 5000;

    // [UPDATE] RAII Allocation
    // Old: Particle* particles = new Particle[NUM_PARTICLES];
    // New: The constructor allocates memory on the Heap immediately.
    // Benefit: If main() crashes or returns early, this memory is auto-freed.
    std::vector<Particle> particles(NUM_PARTICLES);

    // [UPDATE] Initialization Loop
    // We use the range-based loop here too for consistency.
    for (auto& p : particles) {
        // [PERFORMANCE] Slow Random Number Generator
        // 'rand()' is notoriously slow and low-quality.
        // It uses a global lock which kills performance in multithreaded apps.
        p.x = rand() % 500;
        p.y = rand() % 500;
        p.vx = (rand() % 10 - 5) * 5.0f; // Random velocity -5 to 5
        p.vy = (rand() % 10 - 5) * 5.0f; // Random velocity -5 to 5
    }

    long long total_latency_us = 0;
    long long frame_count = 0;

    std::cout << "Starting Legacy Simulation (C++ 98 Style)..." << std::endl;
    std::cout << "Press ESC to quit." << std::endl;

    while (true) {
        auto start_time = std::chrono::high_resolution_clock::now();

        // [UPDATE] Calling the function
        // Old: update_particles(particles, NUM_PARTICLES, 0.1f);
        // New: We just pass the object. No need to pass the size manually.
        update_particles(particles, 0.1f);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        long long current_latency = duration.count();

        total_latency_us += current_latency;
        frame_count++;
        double average_latency = (double)total_latency_us / frame_count;

        // Visualization
        // [PERFORMANCE TIP] We moved the allocation outside the loop in the next step,
        // but for now, we keep it here to match the legacy structure.
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

    // [UPDATE] No Delete
    // Old: delete[] particles;
    // New: (Removed). The vector destructor is called automatically here.

    std::cout << "Simulation finished. Memory cleaned up." << std::endl;

    return 0;
}