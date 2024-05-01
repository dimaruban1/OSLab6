#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

std::mutex mtx;

int shared_variable = 0;
int n_iterations = 1000000;
std::atomic<int> atomic_shared_variable = 0;

void increment_counter_wrong(int& variable, int iterations) {
  for (int i = 0; i < iterations; ++i) {
    variable++;
  }
}

void increment_counter_mutex(int& variable, int iterations) {
  for (int i = 0; i < iterations; ++i) {
    std::lock_guard<std::mutex> lock(mtx);
    variable++;
  }
}

void increment_counter_atomic(int& variable, int iterations) {
  for (int i = 0; i < iterations; ++i) {
    atomic_shared_variable++;
  }
}

void increment_counter_sync(int& variable, int iterations) {
  for (int i = 0; i < iterations; ++i) {
    std::lock_guard<std::mutex> lock(mtx);
    variable++;
  }
}

int main() {
  auto start = std::chrono::high_resolution_clock::now();
  std::thread thread1(increment_counter_wrong, std::ref(shared_variable), n_iterations);
  std::thread thread2(increment_counter_wrong, std::ref(shared_variable), n_iterations);

  thread1.join();
  thread2.join();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "duration is " << duration.count() << " microseconds" << std::endl;
  std::cout << "Кінцеве значення shared_variable: " << shared_variable << std::endl;

  return 0;
}
