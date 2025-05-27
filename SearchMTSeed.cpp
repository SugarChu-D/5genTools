#include <atomic>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <vector>
#include <mutex>
#include "MT_1.cpp"

using namespace std;

int main() {
    uint32_t target = 0x3fffffff;
    vector<uint32_t> found_seeds;
    mutex found_mutex;

    int num_threads = omp_get_max_threads();
    cout << "OpenMP スレッド数: " << num_threads << endl;

    auto start_time = chrono::high_resolution_clock::now();

    #pragma omp parallel for schedule(dynamic, 10000)
    for (uint64_t seed = 0; seed <= 0xFFFFFFFF; ++seed) {
        uint32_t result = MT_32(static_cast<uint32_t>(seed), 0);
        if (result == target) {
            lock_guard<mutex> lock(found_mutex);
            found_seeds.push_back(static_cast<uint32_t>(seed));
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    if (!found_seeds.empty()) {
        cout << "Found seeds:" << endl;
        for (auto s : found_seeds) {
            cout << "  0x" << hex << s << endl;
        }
    } else {
        cout << "Not found." << endl;
    }
    cout << "計算時間: 0x" << duration << " ms" << endl;

    return 0;
}