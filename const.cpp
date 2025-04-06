#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <array>
#include <iomanip>
#include <cstdint>

using namespace std;

class Version {
    string Label; // バージョン名
    array<uint32_t, 5> nazoArray; // nazo値を5つの整数として保持
    uint32_t VCount; // VCountの値

    // ラベルに基づいて値を設定するヘルパー関数
    void initializeValues() {
        static const map<string, pair<array<uint32_t, 5>, uint32_t>> versionData = {  // intからuint32_tに変更
            {"JPB1", {{0x02215F10, 0x0221600C, 0x0221600C, 0x02216058, 0x02216058}, 0x60}},
            {"JPW1", {{0x02215F30, 0x0226020C, 0x0226020C, 0x02216078, 0x02216078}, 0x5F}},
            {"JPB2", {{0x0209A8DC, 0x02039AC9, 0x021FF9B0, 0x021FFA04, 0x021FFA04}, 0x82}},
            {"JPW2", {{0x0209A8FC, 0x02039AF5, 0x021FF9D0, 0x021FFA24, 0x021FFA24}, 0x82}}
        };

        auto it = versionData.find(Label);
        if (it != versionData.end()) {
            nazoArray = it->second.first;
            VCount = it->second.second;
        } else {
            throw invalid_argument("Unknown label: " + Label);
        }
    }

public:
    // コンストラクタ
    Version(string label) : Label(label) {
        initializeValues();
    }

    // ゲッター
    string getLabel() const { return Label; }
    array<uint32_t, 5> getNazoArray() const { return nazoArray; }
    uint32_t getVCount() const { return VCount; }  // 戻り値の型をuint32_tに変更

    // デバッグ用の出力
    void print() const {
        cout << "Version: " << Label << ", VCount: " << hex << VCount << ", nazoArray: ";
        for (const auto& val : nazoArray) {
            cout << hex << val << " ";
        }
        cout << endl;
    }
};

// int main() {
//     const array<string, 4> labels = {"JPB1", "JPW1", "JPB2", "JPW2"};
    
//     try {
//         for (const auto& label : labels) {
//             cout << "\nTesting getNazoArray for label '" << label << "':" << endl;
            
//             // Versionオブジェクトを作成
//             Version version(label);
            
//             // getNazoArrayを呼び出してnazo配列を取得
//             auto nazoArray = version.getNazoArray();
            
//             // 結果を16進数形式で出力
//             cout << "Nazo Array: ";
//             for (const auto& val : nazoArray) {
//                 cout << "0x" << setfill('0') << setw(8) << hex << val << " ";
//             }
//             cout << endl;
            
//             // VCountも表示
//             cout << "VCount: 0x" << hex << version.getVCount() << endl;
//         }
        
//         // エラーケースのテスト
//         cout << "\nTesting invalid label:" << endl;
//         Version invalid("InvalidLabel");
        
//     } catch (const exception& e) {
//         cout << "Expected error caught: " << e.what() << endl;
//     }
    
//     return 0;
// }
