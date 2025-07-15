#include "const.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

const std::map<std::string, char> valid_versions = {
    {"JPB1", '1'}, {"JPW1", '1'},
    {"JPB2", '2'}, {"JPW2", '2'}
    // Add other versions like USB1, USW2 etc. here if needed
};

// コンストラクタの実装
Version::Version(string label) : Label(label) {
    initializeValues();
}

// initializeValuesの実装
void Version::initializeValues() {
    static const map<string, pair<array<uint32_t, 5>, uint32_t>> versionData = {
        {"JPB1", {{0x02215F10, 0x0221600C, 0x0221600C, 0x02216058, 0x02216058}, 0x60}},
        {"JPW1", {{0x02215f30, 0x0221602C, 0x0221602C, 0x02216078, 0x02216078}, 0x5F}},
        {"JPB2", {{0x0209A8DC, 0x02039AC9, 0x021FF9B0, 0x021FFA04, 0x021FFA04}, 0x82}},
        {"JPW2", {{0x0209A8FC, 0x02039AF5, 0x021FF9D0, 0x021FFA24, 0x021FFA24}, 0x82}}
    };

    auto it = versionData.find(Label);
    if (it != versionData.end()) {
        nazoArray = it->second.first;
        VCount = it->second.second;
    } else {
        throw invalid_argument("Unknown version label: " + Label);
    }
}

// ゲッター関数の実装
string Version::getLabel() const {
    return Label;
}

array<uint32_t, 5> Version::getNazoArray() const {
    return nazoArray;
}

uint32_t Version::getVCount() const {
    return VCount;
}

// デバッグ用出力関数の実装
void Version::print() const {
    cout << "Version: " << Label 
         << ", VCount: 0x" << hex << VCount 
         << ", nazoArray: ";
    for (const auto& val : nazoArray) {
        cout << "0x" << setfill('0') << setw(8) << hex << val << " ";
    }
    cout << endl;
}

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
