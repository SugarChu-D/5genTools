#include <atomic>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <vector>
#include <mutex>
#include "MT_1.h"
#include <fstream>
#include <sstream>

using namespace std;

struct SearchConfig {
    uint32_t p_value;
    vector<uint32_t> targets;
};

SearchConfig get_targets_from_file(const string& filename = "ivs.txt") {
    SearchConfig config;
    ifstream file(filename);
    if (!file) {
        throw runtime_error("Could not open configuration file: " + filename);
    }

    string first_line;
    if (!getline(file, first_line)) {
        throw runtime_error("Configuration file is empty.");
    }
    
    stringstream p_ss(first_line);
    if (!(p_ss >> config.p_value) || config.p_value > 20) {
        throw runtime_error("Invalid p value (must be an integer between 0-20): " + first_line);
    }

    string line;
    int line_number = 1;
    
    while (getline(file, line)) {
        line_number++;
        stringstream ss(line);
        vector<int> values;
        int value;
        
        while (ss >> value) {
            if (value >= 0 && value <= 31) {
                values.push_back(value);
            } else {
                throw runtime_error("Invalid setting value (must be an integer between 0-31): " + to_string(value) + " (line: " + to_string(line_number) + ")");
            }
        }

        if (values.size() != 6) {
            throw runtime_error("Each line must contain 6 values (line: " + to_string(line_number) + ", found: " + to_string(values.size()) + ")");
        }

        uint32_t target = 0;
        for (int i = 0; i < 6; i++) {
            target |= (values[i] & 0x1F) << (5 * (5 - i));
        }
        config.targets.push_back(target);
    }

    if (config.targets.empty()) {
        throw runtime_error("No valid values found in the configuration file.");
    }

    return config;
}

int main() {
    try {
        vector<uint32_t> found_seeds;
        mutex found_mutex;
        atomic<bool> should_exit{false};

        ofstream result_file("result.txt");
        ofstream seed_file("ivseed.txt");
        if (!result_file || !seed_file) {
            cerr << "Could not open result file." << endl;
            return 1;
        }

        auto config = get_targets_from_file("ivs.txt");
        cout << "ivRNG: " << config.p_value << endl;
        cout << "targeted:list" << endl;
        result_file << "ivRNG: " << config.p_value << endl;
        result_file << "targeted list:" << endl;
        
        for (size_t i = 0; i < config.targets.size(); i++) {
            cout << "[" << i << "] 0x" << hex << config.targets[i] << endl;
            result_file << "[" << i << "] 0x" << hex << config.targets[i] << endl;
        }

        int num_threads = omp_get_max_threads();
        cout << "OpenMP thread: " << num_threads << endl;
        
        auto start_time = chrono::high_resolution_clock::now();

        #pragma omp parallel for schedule(dynamic, 10000)
        for (int64_t seed = 0; seed <= 0xFFFFFFFF; ++seed) {
            if (should_exit) {
                continue;
            }

            if (seed % 0x4400000 == 0) {
                #pragma omp critical
                {
                    double progress = (static_cast<double>(seed) / 0xFFFFFFFF) * 100;
                    cout << "\r progress: " << fixed << setprecision(2) 
                         << progress << "% (0x" << hex << seed << ")" << flush;
                }
            }

            uint32_t result = MT_32(static_cast<uint32_t>(seed), config.p_value);
            
            for (const auto& target : config.targets) {
                if (result == target) {
                    lock_guard<mutex> lock(found_mutex);
                    found_seeds.push_back(static_cast<uint32_t>(seed));
                    result_file << "target 0x" << hex << target 
                                << " seed 0x" << seed << endl;
                    seed_file << hex << seed << endl;

                    if (found_seeds.size() >= 500) {
                        cout << "\ntoo many seeds. I recommend using 5gensearch directly" << endl;
                        result_file << "too many seeds. I recommend using 5gensearch directly" << endl;
                        seed_file << "too many seeds. I recommend using 5gensearch directly" << endl;
                        should_exit = true;
                        break;
                    }
                }
            }
        }

        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

        cout << "\nExecution time: " << duration/60 << "m" << duration%60 << " s" << endl;
        result_file << "Execution time: 0x" << duration/60 << "m 0x" << duration%60 << "s" << endl;

        if (found_seeds.empty()) {
            cout << "404 not found" << endl;
            result_file << "404 not found" << endl;
            seed_file << "404 not found" << endl;
        }

        result_file.close();
        seed_file.close();

        if (should_exit) {
            return 0;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "unknown error (not std::exception)" << endl;
        return 1;
    }

    return 0;
}