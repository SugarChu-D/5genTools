#include <atomic>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <vector>
#include <mutex>
#include "MT_1.cpp"
#include <fstream>
#include <sstream>

using namespace std;

vector<uint32_t> get_targets_from_file(const string& filename = "ivs.txt") {
    ifstream config(filename);
    if (!config) {
        throw runtime_error("設定ファイルが開けません: " + filename);
    }

    vector<uint32_t> targets;
    string line;
    int line_number = 0;
    
    while (getline(config, line)) {
        line_number++;
        stringstream ss(line);
        vector<int> values;
        int value;
        
        // 1行から6つの値を読み込み
        while (ss >> value) {
            if (value >= 0 && value <= 31) {
                values.push_back(value);
            } else {
                throw runtime_error("無効な設定値（0-31の整数を入力してください）: " + 
                                  to_string(value) + " (行: " + to_string(line_number) + ")");
            }
        }

        if (values.size() != 6) {
            throw runtime_error("1行に6つの値が必要です (行: " + to_string(line_number) + 
                              ", 見つかった値: " + to_string(values.size()) + ")");
        }

        // 6つの値からtargetを生成
        uint32_t target = 0;
        for (int i = 0; i < 6; i++) {
            target |= (values[i] & 0x1F) << (5 * (5 - i));
        }
        targets.push_back(target);
    }

    if (targets.empty()) {
        throw runtime_error("設定ファイルに有効な値が見つかりません");
    }

    return targets;
}

int main() {
    vector<uint32_t> targets;
    vector<uint32_t> found_seeds;
    mutex found_mutex;

    ofstream result_file("result.txt");
    if (!result_file) {
        cerr << "結果ファイルを開けません" << endl;
        return 1;
    }

    try {
        targets = get_targets_from_file();
        cout << "読み込まれたtarget一覧:" << endl;
        result_file << "読み込まれたtarget一覧:" << endl;
        for (size_t i = 0; i < targets.size(); i++) {
            cout << "[" << i << "] 0x" << hex << targets[i] << endl;
            result_file << "[" << i << "] 0x" << hex << targets[i] << endl;
        }
    }
    catch (const exception& e) {
        cerr << "エラー: " << e.what() << endl;
        return 1;
    }

    int num_threads = omp_get_max_threads();
    cout << "OpenMP スレッド数: " << num_threads << endl;
    
    auto start_time = chrono::high_resolution_clock::now();

    // 一回の探索で全targetをチェック
    #pragma omp parallel for schedule(dynamic, 10000)
    for (uint64_t seed = 0; seed <= 0x0FFFFFFF; ++seed) {
        // 進捗表示
        if (seed % 0x4400000 == 0) {
            #pragma omp critical
            {
                double progress = (static_cast<double>(seed) / 0x0FFFFFFF) * 100;
                cout << "\r進捗: " << fixed << setprecision(2) 
                     << progress << "% (0x" << hex << seed << ")" << flush;
            }
        }

        uint32_t result = MT_32(static_cast<uint32_t>(seed), 0);
        
        // 全targetと比較
        for (const auto& target : targets) {
            if (result == target) {
                lock_guard<mutex> lock(found_mutex);
                found_seeds.push_back(static_cast<uint32_t>(seed));
                // targetとseedの対応を記録
                cout << "\nFound target 0x" << hex << target 
                     << " with seed 0x" << seed << endl;
                result_file << "target 0x" << hex << target 
                          << " seed 0x" << seed << endl;
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "\n計算時間: 0x" << hex << duration << " ms" << endl;

    if (found_seeds.empty()) {
        cout << "見つかりませんでした" << endl;
        result_file << "見つかりませんでした" << endl;
    }

    result_file.close();
    return 0;
}