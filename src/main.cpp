#include <iostream>
#include <cstdlib>
// #include <vector>
#include <opencv2/opencv.hpp>

struct Particle {
    float x, y;
    float vx, vy;
};

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
    const int NUM_PARTICLES = 1000;

    Particle* particles = new Particle[NUM_PARTICLES];

    for (int i = 0; i < NUM_PARTICLES; ++i) {
        particles[i].x = rand() % 500;
        particles[i].y = rand() % 500;
        particles[i].vx = (rand() % 10 - 5) * 1.0f; // Random velocity -5 to 5
        particles[i].vy = (rand() % 10 - 5) * 1.0f; // Random velocity -5 to 5
    }

    std::cout << "Starting Legacy Simulation (C++ 98 Style)..." << std::endl;
    std::cout << "Press ESC to quit." << std::endl;

    while (true) {
        // Update physics
        update_particles(particles, NUM_PARTICLES, 0.1f);

        // Visualization (OpenCV)
        cv::Mat frame = cv::Mat::zeros(500, 500, CV_8UC3);

        for (int i = 0; i < NUM_PARTICLES; ++i) {
            cv::circle(frame, cv::Point(particles[i].x, particles[i].y), 2, cv::Scalar(0, 0, 255), -1);
        }

        cv::imshow("Legacy Sim", frame);
        if (cv::waitKey(16) == 27) break;   // 27 is ESC
    }

    delete[] particles;

    std::cout << "Simulation finished. Memory cleaned up." << std::endl;

    return 0;
}