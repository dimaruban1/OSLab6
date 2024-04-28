#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

mutex mtx;

void multiply_element(int i, int j, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
  int sum = 0;
  for (int p = 0; p < B.size(); ++p) {
    sum += A[i][p] * B[p][j];
  }

  mtx.lock();
  C[i][j] = sum;
  mtx.unlock();
//  cout << "Thread " << this_thread::get_id() << ": C[" << i << "," << j << "] = " << sum << endl;
}

void less_by_n_threads(const vector<pair<int, int>>& indices, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    for (const auto& idx : indices) {
        multiply_element(idx.first, idx.second, A, B, C);
    }
}

// Function to create threads to perform calculations with fewer threads
int create_threads(int n, int k, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int num_less) {
    auto start = chrono::high_resolution_clock::now();
    vector<pair<int, int>> indices;
    for (int i = 0; i < n; i += num_less) {
        for (int j = 0; j < k; ++j) {
            for (int l = 0; l < num_less && i + l < n; ++l) {
                indices.emplace_back(i + l, j);
            }
            less_by_n_threads(indices, A, B, C);
            indices.clear();
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    return duration.count();
}

int main() {
    int n = 10;
    int m = 15;
    int k = 8;

    // Example matrices A and B (10x15 and 15x8 respectively)
    vector<vector<int>> A(n, vector<int>(m, 1)); // Filling with 1s for simplicity
    vector<vector<int>> B(m, vector<int>(k, 1));

    // Initialize matrix C with zeros
    vector<vector<int>> C(n, vector<int>(k, 0));

    // Create threads to perform calculations with fewer threads
    create_threads(n, k, A, B, C, 1); // Using five_time_less_threads
    int divider = 5;
    cin >> divider;
    double res = 0;
    for (int i = 0; i < 10000; i++) {
        res += create_threads(n , k, A, B, C, divider);
    }
    res = res / 10000;
    cout << "average duration is " << res << " microseconds in 10000 attempts" << endl;
    cout << "number of threads is (approximately)" << n*k / divider << endl;
    return 0;
}