#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

// Use atomics for thread-safe counter updates
std::atomic<int> shared_variable = 0;
int n_iterations = 1000000;

void increment_counter(std::atomic<int>& variable, int iterations) {
  for (int i = 0; i < iterations; ++i) {
    // Directly increment the atomic variable
    variable++;  // Thread-safe increment
  }
}

int main() {
  std::thread thread1(increment_counter, std::ref(shared_variable), n_iterations);
  std::thread thread2(increment_counter, std::ref(shared_variable), n_iterations);

  thread1.join();
  thread2.join();

  std::cout << "Final value of shared_variable (expected to be non-deterministic): " << shared_variable << std::endl;

  return 0;
}