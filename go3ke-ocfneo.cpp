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

int main() {
    vector<uint32_t> found_seeds;
    mutex found_mutex;
    atomic<bool> should_exit{false};

    // 結果出力用ファイル
    ofstream result_file("result_6V.txt");
    if (!result_file) {
        cerr << "結果ファイルを開けません" << endl;
        return 1;
    }

    // 固定パラメータの設定
    Version version("JPB1");
    uint16_t Timer0 = 0x0C7A;
    bool isDSLite = false;
    uint64_t MAC = 0x0009BF6D93CE;

    // パラメータの構造体を作成
    InitialSeedParams params = {
        version,
        Timer0,
        isDSLite,
        MAC
    };

    auto start_time = chrono::high_resolution_clock::now();

    // 全探索の総数を計算（100年 × 0x1000個のkeypress）
    uint64_t total_iterations = 100ULL * 24 * 60 * 0x1000;

    #pragma omp parallel
    {
        SHA1_DATA sha1d = {};
        #pragma omp for schedule(dynamic, 1000) collapse(5)
        for (int year = 0; year < 100; ++year) {
            for (int hour = 0; hour < 24; ++hour) {
                for (int minute = 0; minute < 60; ++minute) {
                    for (int second = 5; second < 11; ++second) {
                        for (uint16_t keypress = 0x2000; keypress <= 0x2fff; ++keypress) {
                            if (should_exit) continue;

                            // 8月31日で固定、年のみ変更
                                GameDate gameDate(year, 8, 31, hour, minute, second);

                            // 進捗表示（年とkeypressの組み合わせから計算）
                            if ((year * 0x1000 * 24 * 60 + (keypress - 0x2000)) % 0x100000 == 0) {
                                #pragma omp critical
                                {
                                    double progress = (static_cast<double>(year * 0x1000 * 24 * 60 + (keypress - 0x2000)) / total_iterations) * 100;
                                    cout << "\rprogress: " << fixed << setprecision(2) 
                                         << progress << "% (Year: " << dec << 2000 + year 
                                         << ", Keypress: 0x" << hex << keypress << ")" << flush;
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
                    
                    // cout << "\n[Found] Year: " << dec << 2000 + year 
                    //      << ", Keypress: 0x" << hex << keypress 
                    //      << " (" << bitset<16>(keypress) << ") "
                    //      << "Keys: ";
                    // for (const auto& key : keys) {
                    //     cout << key << " ";
                    // }
                    // cout << "\n-> Initial Seed: 0x" << result
                    //      << " -> Next Value Upper32: 0x" << upper32 << endl;

                                result_file << dec << 2000 + year << "/8/31 "
                                            << hour << ":" << minute << ":" << second
                                            << " Keypress: 0x" << hex << keypress 
                                            << " Keys: ";
                            for (const auto& key : keys) {
                                result_file << key << " ";
                            }
                            result_file << " Initial Seed: 0x" << result 
                                        << " Next Value Upper32: 0x" << upper32 << endl;
                        }
                    }
                }
            }
        }
    }
}

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
    
    cout << "\n実行時間: 0x" << duration/60 << "m 0x" << duration%60 << "s" << endl;
    cout << "found: " << found_seeds.size() << endl;

    result_file.close();
    return 0;
}
