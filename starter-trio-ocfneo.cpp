#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <atomic>
#include <unordered_set>
#include <omp.h>
#include "sha-1.cpp"
#include "const.h"
#include "GameDate.h"

// 配列ベースのkeypresses.cppを追加
#include "keypresses_array_generated.cpp"

using namespace std;

// ターゲットとなる上位32ビット値の定数セット
const unordered_set<uint32_t> target_seeds = {
    0x09098ab6, 0x1555fe58,  0x2dd79610, 0x3ecbda32, 0x963be2b3, 0xaa64dfdd, 0xc13a6677, 0xdfa564da
};

// 探索対象の日付を定義
struct SearchDate {
    int month;
    int day;
    string name;
};

const vector<SearchDate> search_dates = {
    {4, 30, "4/30"},
    {8, 31, "8/31"},
    {12, 31, "12/31"}
};

int main() {
    vector<uint32_t> found_seeds;
    mutex found_mutex;
    atomic<bool> should_exit{false};

    // 結果出力用ファイル
    ofstream result_file("result_6V.txt");
    if (!result_file) {
        cerr << "cannot open result_file" << endl;
        return 1;
    }

    // config.txtからパラメータを読み込む
    ifstream config_file("config.txt");
    if (!config_file) {
        cerr << "cannot open config.txt" << endl;
        return 1;
    }
    
    string version_str, timer0_str, isDSLite_str, mac_str;
    if (!getline(config_file, version_str) || 
        !getline(config_file, timer0_str) || 
        !getline(config_file, isDSLite_str) || 
        !getline(config_file, mac_str)) {
        cerr << "failed to read config.txt" << endl;
        return 1;
    }
    config_file.close();

    // パラメータの検証と変換
    try {
        Version version(version_str);
        uint16_t Timer0 = static_cast<uint16_t>(stoul(timer0_str, nullptr, 16));
        bool isDSLite = (isDSLite_str == "true" || isDSLite_str == "1");
        uint64_t MAC = stoull(mac_str, nullptr, 16);

        // パラメータの構造体を作成
        InitialSeedParams params = {
            version,
            Timer0,
            isDSLite,
            MAC
        };

    // 配列ベースの統計情報を表示
    printKeypressStats();

    // 総反復回数の計算（最適化版）
    uint64_t total_iterations = 100 * 3 * 24 * 60 * 20 * VALID_KEYPRESS_COUNT;
    cout << "Total iterations: " << total_iterations << " (" 
         << (total_iterations / 1000000) << "M iterations)" << endl;
    cout << "Starting parallel search with " << omp_get_max_threads() << " threads..." << endl;
    cout << "Target seeds: " << target_seeds.size() << " values" << endl;

    auto start_time = chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        SHA1_DATA sha1d = {};
        #pragma omp for schedule(dynamic, 10000) collapse(6)
        for (int date_idx = 0; date_idx < 3; ++date_idx) {
            for (int year = 0; year < 100; ++year) {
                for (int hour = 0; hour < 23; ++hour) {
                    for (int minute = 0; minute < 60; ++minute) {
                        for (int second = 5; second < 25; ++second) {
                            for (size_t keypress_idx = 0; keypress_idx < VALID_KEYPRESS_COUNT; ++keypress_idx) {
                                if (should_exit) continue;

                                const SearchDate& current_date = search_dates[date_idx];
                                GameDate gameDate(year, current_date.month, current_date.day, hour, minute, second);
                                uint16_t keypress = VALID_KEYPRESSES[keypress_idx];

                                // 進捗表示（頻度を調整）
                                uint64_t current_iteration = 
                                    static_cast<uint64_t>(date_idx) * 100 * 23 * 60 * 20 * VALID_KEYPRESS_COUNT +
                                    static_cast<uint64_t>(year) * 23 * 60 * 20 * VALID_KEYPRESS_COUNT +
                                    static_cast<uint64_t>(hour) * 60 * 20 * VALID_KEYPRESS_COUNT +
                                    static_cast<uint64_t>(minute) * 20 * VALID_KEYPRESS_COUNT +
                                    static_cast<uint64_t>(second - 5) * VALID_KEYPRESS_COUNT +
                                    static_cast<uint64_t>(keypress_idx);

                                if (current_iteration % (0x1000000) == 0) {
                                    #pragma omp critical
                                    {
                                        double progress = (static_cast<double>(current_iteration) / total_iterations) * 100;
                                        cout << "\rprogress: " << fixed << setprecision(2) 
                                             << progress << "% (Date: " << current_date.name
                                             << ", Year: " << dec << 2000 + year 
                                             << ", Hour: " << hour << ")" << flush;
                                    }
                                }

                                uint64_t result = initialSEED(&sha1d, params, gameDate, keypress);
                    
                                // LCGで次の値を生成し、上位32ビットを取得
                                uint64_t next_value = result * 0x5D588B656C078965UL + 0x269EC3UL;
                                uint32_t upper32 = static_cast<uint32_t>(next_value >> 32);

                                // ターゲット値と比較
                                if (target_seeds.find(upper32) != target_seeds.end()) {
                                    lock_guard<mutex> lock(found_mutex);
                                    found_seeds.push_back(keypress);
                        
                                    // キー入力の解析を追加
                                    vector<string> keys = getKeysFromBits(keypress);
                        
                                    // 結果を即座にファイルに出力
                                    result_file << dec << year << "," << current_date.month << "," << current_date.day << ","
                                                << hour << "," << minute << "," << second
                                                << "," << hex << next_value << ",";
                                    for (const auto& key : keys) {
                                        result_file << key << " ";
                                    }
                                    result_file << endl;
                                    result_file.flush(); // 即座にフラッシュ
                                    
                                    // コンソールにも表示
                                    cout << "\n[Found] Date: " << current_date.name << "/" << 2000 + year 
                                         << " " << hour << ":" << minute << ":" << second
                                         << " Keypress: 0x" << hex << keypress 
                                         << " Seed: 0x" << next_value << endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
    
    cout << "\n=== Search completed ===" << endl;
    cout << "Execution time: " << duration/60 << " minutes " << duration%60 << " seconds" << endl;
    cout << "Found combinations: " << found_seeds.size() << " values" << endl;
    cout << "Result file: result_6V.txt" << endl;

    result_file.close();
    return 0;
    
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}