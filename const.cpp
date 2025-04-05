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
    int VCount; // VCountの値

    // ラベルに基づいて値を設定するヘルパー関数
    void initializeValues() {
        static const map<string, pair<array<uint32_t, 5>, int>> versionData = {
            {"JPB1", {{0x02215F10, 0x0221600C, 0x0221600C, 0x02216058, 0x02216058}, 0x60}},
            {"JPW1", {{0x02215F30, 0x0226020C, 0x0226020C, 0x0226078, 0x0226078}, 0x5F}},
            {"JPB2", {{0x209A8DC, 0x2039AC9, 0x21FF9B0, 0x21FFA04, 0x21FFA04}, 0x82}},
            {"JPW2", {{0x209A8FC, 0x2039AF5, 0x21FF9D0, 0x21FFA24, 0x21FFA24}, 0x82}}
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
    int getVCount() const { return VCount; }

    // デバッグ用の出力
    void print() const {
        cout << "Version: " << Label << ", VCount: " << hex << VCount << ", nazoArray: ";
        for (const auto& val : nazoArray) {
            cout << hex << val << " ";
        }
        cout << endl;
    }
};

int main() {
    char Data[2048] = {0}; // 初期化
    try {
        // ラベルに基づいてオブジェクトを作成
        Version v1("JPB2");
        v1.print();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
