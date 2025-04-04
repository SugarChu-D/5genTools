#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

class Version {
    string Label; // バージョン名
    vector<int> nazo; // nazo値（5つの整数）
    int VCount; // VCountの値

    // ラベルに基づいて値を設定するヘルパー関数
    void initializeValues() {
        if (Label == "JPB1") {
            nazo = {0x2215f10, 0x221600C, 0x221600C, 0x2216058, 0x2216058};
            VCount = 0x60;
        } else if (Label == "JPW1") {
            nazo = {0x2215f30, 0x221602C, 0x221602C, 0x2216078, 0x2216078};
            VCount = 0x5F;
        } else if (Label == "JPB2") {
            nazo = {0x209A8DC, 0x2039AC9, 0x21FF9B0, 0x21FFA04, 0x21FFA04};
            VCount = 0x82;
        } else if (Label == "JPW2") {
            nazo = {0x209A8FC, 0x2039AF5, 0x21FF9D0, 0x21FFA24, 0x21FFA24};
            VCount = 0x82;
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
    vector<int> getNazo() const { return nazo; }
    int getVCount() const { return VCount; }

    // デバッグ用の出力
    void print() const {
        cout << "Version: " << Label << ", VCount: " << VCount << ", nazo: ";
        for (int value : nazo) {
            cout << value << " ";
        }
        cout << endl;
    }
};

int main() {
    try {
        // ラベルに基づいてオブジェクトを作成
        Version v1("JPB1");
        v1.print();

        Version v2("JPB2");
        v2.print();

        // 未知のラベルを渡した場合
        Version v4("Unknown");
        v4.print();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
