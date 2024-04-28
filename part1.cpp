#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

mutex mtx;

vector<vector<int>> generate_random_matrix(int rows, int cols) {
    vector<vector<int>> matrix(rows, vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = rand() % 10 + 1;  // 1 to 10
        }
    }
    return matrix;
}

void print_matrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

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

void twice_less_threads(int i1, int j1, int i2, int j2, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    multiply_element(i1, j1, A, B, C);
    multiply_element(i2, j2, A, B, C);
}

void less_threads(const vector<pair<int, int>>& indices, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    for (const auto& idx : indices) {
        cout << "multiplying " << idx.first << " " << idx.second << endl;
        multiply_element(idx.first, idx.second, A, B, C);
    }
}

int testnk(int n, int k, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    auto start = chrono::high_resolution_clock::now();
    vector <thread> threads;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            threads.emplace_back(multiply_element, i, j, ref(A), ref(B), ref(C));
        }
    }

    for (auto& th : threads) {
        th.join();
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "duration is " << duration.count() << " microseconds" << endl;
    cout << "number of threads is " << n*k << endl;
    return duration.count();
}

int testnk_by2(int n, int k, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    auto start = chrono::high_resolution_clock::now();
    vector <thread> threads;
    for (int i = 0; i < n; i += 2) {
        for (int j = 0; j < k; ++j) {
            if (i + 1 < n) {
                threads.emplace_back(twice_less_threads, i, j, i + 1, j, ref(A), ref(B), ref(C));
            } else {
                threads.emplace_back(multiply_element, i, j, ref(A), ref(B), ref(C));
            }
        }
    }

    for (auto& th : threads) {
        th.join();
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "duration is " << duration.count() << " microseconds" << endl;
    cout << "number of threads is " << n*k << endl;
    return duration.count();
}

int testnk_by5(int n, int k, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    auto start = chrono::high_resolution_clock::now();
    vector <thread> threads;
    vector<pair<int, int>> indices;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; ++j) {
            cout << i << " " << j << endl;
            indices.clear();
            if (j + 4 < n) {
                j+=4;
                vector<pair<int, int>> indices;
                indices.push_back(make_pair(i, j));
                indices.push_back(make_pair(i, j+1));
                indices.push_back(make_pair(i, j+2));
                indices.push_back(make_pair(i, j+3));
                indices.push_back(make_pair(i, j+4));

                threads.emplace_back(less_threads, ref(indices), ref(A), ref(B), ref(C));
            } else {
                threads.emplace_back(multiply_element, i, j, ref(A), ref(B), ref(C));
            }
        }
    }

    for (auto& th : threads) {
        th.join();
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "duration is " << duration.count() << " microseconds" << endl;
    cout << "number of threads is " << n*k << endl;
    return duration.count();
}

// 1096
// 551 
int main() {
    int n = 10;
    int m = 15;
    int k = 8;

    vector<vector<int>> A = generate_random_matrix(n, m);
    vector<vector<int>> B = generate_random_matrix(m, k);

    // cout << "Matrix A:" << endl;
    // print_matrix(A);
    // cout << endl << "Matrix B:" << endl;
    // print_matrix(B);

    vector<vector<int>> C(n, vector<int>(k, 0));
    
    // auto start = chrono::high_resolution_clock::now();
    // vector <thread> threads;
    // for (int i = 0; i < n; ++i) {
    //     for (int j = 0; j < k; ++j) {
    //         // Create a thread for each element of matrix C
    //         threads.emplace_back(multiply_element, i, j, ref(A), ref(B), ref(C));
    //     }
    // }

    // // Wait for all threads to finish
    // for (auto& th : threads) {
    //     th.join();
    // }
    // auto stop = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    // cout << "duration is " << duration.count() << " microseconds" << endl;
    // cout << "number of threads is " << n*k << endl;

    // cout << "Matrix C:" << endl;
    // print_matrix(C);
    double res = 0;
    for (int i = 0; i < 10000; i++) {
        res += testnk_by5(n , k, A, B, C);
    }
    res = res / 10000;
    cout << "average duration is " << res << " microseconds in 10000 attempts" << endl;
    cout << "number of threads is " << n*k << endl;
    return 0;
}
