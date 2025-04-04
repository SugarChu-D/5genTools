#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <map>
#include <array>
#include "sha-1.h"
#include <cstring>
#include <iomanip>

using namespace std;

class Version {
    string Label; // バージョン名
    string nazo; // nazo値（5つの整数をガッチャンコした文字列）
    int VCount; // VCountの値

    // ラベルに基づいて値を設定するヘルパー関数
    void initializeValues() {
        static const map<string, pair<string, int>> versionData = {
            {"JPB1", {"105f21020c6021020c6021025860210258602102", 0x60}}, //{0x02215f10, 0x0221600C, 0x0221600C, 0x02216058, 0x02216058}
            {"JPW1", {"305f21022c6021022c6021027860210278602102", 0x5F}}, //{0x02215f30, 0x022602C, 0x022602C, 0x0226078, 0x0226078}
            {"JPB2", {"dca80902c99a0302b0f91f0204fa1f0204fa1f02", 0x82}}, //{0x209A8DC, 0x2039AC9, 0x21FF9B0, 0x21FFA04, 0x21FFA04}
            {"JPW2", {"fca80902f59a0302d0f91f0224fa1f0224fa1f02", 0x82}} //{0x209A8FC, 0x2039AF5, 0x21FF9D0, 0x21FFA24, 0x21FFA24}
        };

        auto it = versionData.find(Label);
        if (it != versionData.end()) {
            nazo = it->second.first;
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
    string getNazo() const { return nazo; }
    int getVCount() const { return VCount; }

    // デバッグ用の出力
    void print() const {
        cout << "Version: " << Label << ", VCount: " << hex << VCount << ", nazo: " << nazo << endl;
    }
};

// int main() {
//     char Data[2048] = {0}; // 初期化
//     try {
//         // ラベルに基づいてオブジェクトを作成
//         Version v1("JPB2");
//         v1.print();
//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//     }

//     return 0;
// }
