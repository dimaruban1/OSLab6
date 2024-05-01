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

void multiply_element(int i, int j, vector<vector<int>>A, vector<vector<int>> B, vector<vector<int>> C) {
  int sum = 0;
  for (int p = 0; p < B.size(); ++p) {
    sum += A[i][p] * B[p][j];
  }

  mtx.lock();
  C[i][j] = sum;
  mtx.unlock();
//  cout << "Thread " << this_thread::get_id() << ": C[" << i << "," << j << "] = " << sum << endl;
}


void multiply_N_elements(int i0, int j0, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int N) {
    int n = A.size();
    int k = B.front().size();
    for (int i = i0; i < n; ++i) {
        for (int j = j0; j < k && j < N; ++j) {
            multiply_element(i, j, A, B, C);
        }
    }
}

int testnk_byN(int n, int k, const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int N) {
    auto start = chrono::high_resolution_clock::now();
    vector <thread> threads;


    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; j+=N) {
            threads.emplace_back(multiply_N_elements, i, j, ref(A), ref(B), ref(C), N);
        }
    }



    for (auto& th : threads) {
        th.join();
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
//    cout << "duration is " << duration.count() << " microseconds" << endl;
//    cout << "number of threads is " << n*k << endl;
    return duration.count();
}

int main() {
    int n = 18;
    int m = 18;
    int k = 18;

    vector<vector<int>> A = generate_random_matrix(n, m);
    vector<vector<int>> B = generate_random_matrix(m, k);

    vector<vector<int>> C(n, vector<int>(k, 0));
    
    double res = 0;
    for (int n1 = 1; n1 < 10; n1++) {        
        for (int i = 0; i < 1000; i++) {
            res += testnk_byN(n , k, ref(A), ref(B), ref(C), n1);
        }
        res = res / 10000;
        cout << "average duration is " << res << " microseconds in 10000 attempts" << endl;
        cout << "number of threads is " << n*k / n1 << endl;
    }

    return 0;
}

/*
int n = 100;
int m = 90;
int k = 80;
average duration is 137.735 microseconds in 10000 attempts
number of threads is 8000
average duration is 151.37 microseconds in 10000 attempts
number of threads is 4000
average duration is 186.79 microseconds in 10000 attempts
number of threads is 2666
average duration is 227.457 microseconds in 10000 attempts
number of threads is 2000
average duration is 280.407 microseconds in 10000 attempts
number of threads is 1600
average duration is 335.304 microseconds in 10000 attempts
number of threads is 1333
average duration is 389.276 microseconds in 10000 attempts
number of threads is 1142
average duration is 447.323 microseconds in 10000 attempts
number of threads is 1000
average duration is 522.878 microseconds in 10000 attempts
number of threads is 888

int n = 10;
int m = 20;
int k = 30;
average duration is 6.4536 microseconds in 10000 attempts
number of threads is 300
average duration is 2.28455 microseconds in 10000 attempts
number of threads is 150
average duration is 1.54483 microseconds in 10000 attempts
number of threads is 100
average duration is 1.15685 microseconds in 10000 attempts
number of threads is 75
average duration is 1.19212 microseconds in 10000 attempts
number of threads is 60
average duration is 1.48622 microseconds in 10000 attempts
number of threads is 50
average duration is 1.41065 microseconds in 10000 attempts
number of threads is 42
average duration is 1.07644 microseconds in 10000 attempts
number of threads is 37
average duration is 1.35181 microseconds in 10000 attempts
number of threads is 33

int n = 50;
int m = 30;
int k = 40;
average duration is 29.6967 microseconds in 10000 attempts
number of threads is 2000
average duration is 23.9086 microseconds in 10000 attempts
number of threads is 1000
average duration is 27.989 microseconds in 10000 attempts
number of threads is 666
average duration is 31.1186 microseconds in 10000 attempts
number of threads is 500
average duration is 34.5858 microseconds in 10000 attempts
number of threads is 400
average duration is 39.1238 microseconds in 10000 attempts
number of threads is 333
average duration is 50.0067 microseconds in 10000 attempts
number of threads is 285
average duration is 50.3489 microseconds in 10000 attempts
number of threads is 250
average duration is 56.2598 microseconds in 10000 attempts
number of threads is 222

int n = 18;
int m = 18;
int k = 18;
average duration is 430.008 microseconds in 10000 attempts
number of threads is 324
average duration is 237.092 microseconds in 10000 attempts
number of threads is 162
average duration is 179.856 microseconds in 10000 attempts
number of threads is 108
average duration is 236.633 microseconds in 10000 attempts
number of threads is 81
average duration is 241.788 microseconds in 10000 attempts
number of threads is 64
average duration is 271.671 microseconds in 10000 attempts
number of threads is 54
average duration is 306.009 microseconds in 10000 attempts
number of threads is 46
average duration is 344.779 microseconds in 10000 attempts
number of threads is 40
average duration is 355.716 microseconds in 10000 attempts
number of threads is 36
*/