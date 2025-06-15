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

// keypresses.cppを追加
#include "keypresses.cpp"

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
    {4, 29, "4/29"},
    {8, 30, "8/30"},
    {12, 30, "12/30"}
};

int main() {
    vector<uint32_t> found_seeds;
    mutex found_mutex;
    atomic<bool> should_exit{false};

    // 結果出力用ファイル
    ofstream result_file("result_6Vn.txt");
    if (!result_file) {
        cerr << "結果ファイルを開けません" << endl;
        return 1;
    }

    // 固定パラメータの設定
    Version version("JPW1");
    uint16_t Timer0 = 0x0C68;
    bool isDSLite = false;
    uint64_t MAC = 0x0009BF6D93CE;

    // パラメータの構造体を作成
    InitialSeedParams params = {
        version,
        Timer0,
        isDSLite,
        MAC
    };

    // KeypressManagerの統計情報を表示
    keypressManager.printStats();

    auto start_time = chrono::high_resolution_clock::now();

    // 全探索の総数を計算（100年 × 3日付 × 有効keypress数）
    uint64_t total_iterations = 100ULL * 3 * 2 * 60 * 6 * keypressManager.size();
    
    cout << "Total iterations: " << total_iterations << endl;
    cout << "Starting parallel search..." << endl;

    #pragma omp parallel
    {
        SHA1_DATA sha1d = {};
        #pragma omp for schedule(dynamic, 1000) collapse(6)
        for (int date_idx = 0; date_idx < 3; ++date_idx) {
            for (int year = 0; year < 100; ++year) {
                for (int hour = 22; hour < 24; ++hour) {
                    for (int minute = 0; minute < 60; ++minute) {
                        for (int second = 5; second < 18; ++second) {
                            for (size_t keypress_idx = 0; keypress_idx < keypressManager.size(); ++keypress_idx) {
                                if (should_exit) continue;

                                const SearchDate& current_date = search_dates[date_idx];
                                GameDate gameDate(year, current_date.month, current_date.day, hour, minute, second);
                                uint16_t keypress = keypressManager[keypress_idx];

                                // 進捗表示
                                uint64_t current_iteration = 
                                    static_cast<uint64_t>(date_idx) * 100 * 2 * 60 * 6 * keypressManager.size() +
                                    static_cast<uint64_t>(year) * 2 * 60 * 6 * keypressManager.size() +
                                    static_cast<uint64_t>(hour) * 60 * 6 * keypressManager.size() +
                                    static_cast<uint64_t>(minute) * 6 * keypressManager.size() +
                                    static_cast<uint64_t>(second - 5) * keypressManager.size() +
                                    static_cast<uint64_t>(keypress_idx);

                                if (current_iteration % (0x100000 * 24) == 0) {
                                    #pragma omp critical
                                    {
                                        double progress = (static_cast<double>(current_iteration) / total_iterations) * 100;
                                        cout << "\rprogress: " << fixed << setprecision(2) 
                                             << progress << "% (Date: " << current_date.name
                                             << ", Year: " << dec << 2000 + year 
                                             << ", hour: "  << hour << ")" << flush;
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
                        
                                    result_file << dec << year << "," << current_date.month << "," << current_date.day << ","
                                                << hour << "," << minute << "," << second
                                                << "," << hex << next_value << ",";
                                    for (const auto& key : keys) {
                                        result_file << key << " ";
                                    }
                                    result_file << endl;
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
    
    cout << "\n実行時間: " << duration/60 << dec << "m " << duration%60 << "s" << endl;
    cout << "Found seeds: " << found_seeds.size() << endl;

    result_file.close();
    return 0;
}