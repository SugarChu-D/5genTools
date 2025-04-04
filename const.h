#ifndef CONST_H
#define CONST_H

#include <string>
#include <array>
#include <stdexcept>
#include <map>

using namespace std;

class Version {
    string Label; // バージョン名
    string nazo; // nazo値（5つの整数をガッチャンコした文字列）
    int VCount; // VCountの値

    // ラベルに基づいて値を設定するヘルパー関数
    void initializeValues();

public:
    // コンストラクタ
    Version(string& label);

    // ゲッター
    string getLabel() const;
    string getNazo() const;
    int getVCount() const;

    // デバッグ用の出力
    void print() const;
};

#endif // CONST_H