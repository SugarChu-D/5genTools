#ifndef CONST_H
#define CONST_H

#include <string>
#include <array>
#include <map>

using namespace std;

class Version {
private:
    string Label;                    // バージョン名
    array<uint32_t, 5> nazoArray;   // nazo値を5つの整数として保持
    uint32_t VCount;                // VCountの値

    // ラベルに基づいて値を設定するヘルパー関数
    void initializeValues();

public:
    // コンストラクタ
    Version(string label);

    // ゲッター
    string getLabel() const;
    array<uint32_t, 5> getNazoArray() const;
    uint32_t getVCount() const;

    // デバッグ用の出力
    void print() const;
};

#endif // CONST_H