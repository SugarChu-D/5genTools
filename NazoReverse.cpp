#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <array>
#include <iomanip>

using namespace std;

array<uint32_t, 5> nazo = {0x02215f10, 0x0221600C, 0x0221600C, 0x02216058, 0x02216058};
size_t dataIndex = 0; // Data 配列のインデックス

int main() {
    char Data[2048] = {0}; // 初期化

    for (int i = 0; i < 5; i++) {
        stringstream ss;
        ss << hex << nazo[i]; // nazo の値を16進数文字列に変換
        string nazoHex = ss.str();

        // 奇数桁の場合、頭に '0' を追加
        if (nazoHex.size() % 2 != 0) {
            nazoHex = "0" + nazoHex;
        }

        // 後ろから2桁ずつ取り出して Data に格納
        for (int j = nazoHex.size() - 2; j >= 0; j -= 2) {
            if (dataIndex >= sizeof(Data) - 1) {
                cerr << "Error: Data buffer overflow" << endl;
                return 1;
            }
            Data[dataIndex++] = nazoHex[j];
            Data[dataIndex++] = nazoHex[j + 1];
        }
    }

    Data[dataIndex] = '\0'; // 終端文字を追加

    cout << "Data (under 2048 bytes) = " << Data << endl;

    return 0;
}