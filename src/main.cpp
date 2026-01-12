#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <chrono> // Standard for measuring time (C++11)

struct Particle {
    float x, y;
    float vx, vy;
};

// C++98 Style: Raw pointer + Size
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
    // INCREASED COUNT: To see measurable lag, let's use more particles!
    // 1,000 is too fast for modern CPUs to measure accurately.
    const int NUM_PARTICLES = 5000;

    Particle* particles = new Particle[NUM_PARTICLES];

    for (int i = 0; i < NUM_PARTICLES; ++i) {
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
        // 1. Start Timer
        auto start_time = std::chrono::high_resolution_clock::now();

        // Update physics
        update_particles(particles, NUM_PARTICLES, 0.1f);

        // 2. Stop Timer
        auto end_time = std::chrono::high_resolution_clock::now();
        // Calculate duration in microseconds (us)
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        long long current_latency = duration.count();

        // 3. Update Average
        total_latency_us += current_latency;
        frame_count++;
        double average_latency = (double)total_latency_us / frame_count;

        // Visualization (OpenCV)
        // Note: Drawing is slow, so we measure physics BEFORE drawing.
        cv::Mat frame = cv::Mat::zeros(500, 500, CV_8UC3);

        for (int i = 0; i < NUM_PARTICLES; ++i) {
            // Simple color logic based on index
            cv::Scalar color = (i < NUM_PARTICLES/2) ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 255);
            cv::circle(frame, cv::Point(particles[i].x, particles[i].y), 2, color, -1);
        }

        // Draw Metrics: Instant vs Average
        std::string instant_text = "Instant: " + std::to_string(current_latency) + " us";
        std::string avg_text     = "Average: " + std::to_string((int)average_latency) + " us";

        // Green text for instant, White for average
        cv::putText(frame, instant_text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, avg_text,     cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

        cv::imshow("Legacy Sim", frame);
        if (cv::waitKey(16) == 27) break;   // 27 is ESC
    }

    delete[] particles;

    std::cout << "Simulation finished. Memory cleaned up." << std::endl;

    return 0;
}