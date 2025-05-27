#include "MT_1.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;

uint32_t MT_1(uint64_t seed1, uint32_t p) {
    uint16_t length = p + 6 + M;
    if (length > N) {
        throw invalid_argument("Length must be between 0 and N (624).");
    }
    if (p >= N) {
        throw invalid_argument("p must be between 0 and N (624).");
    }

    // メモリアロケーションを1回だけに
    vector<uint32_t> table(length);
    table[0] = static_cast<uint32_t>(seed1 >> 32);

    // 初期化ループの最適化
    uint32_t prev = table[0];
    for(uint32_t i = 1; i < length; ++i) {
        prev = table[i] = 1812433253U * (prev ^ (prev >> 30)) + i;
    }

    uint32_t ivsCode = 0;
    
    // メインループの最適化
    for(int i = p, shift = 0; i < p + 6; ++i, shift += 5) {
        uint32_t x = (table[i] & UPPER_MASK) | (table[i + 1] & LOWER_MASK);
        uint32_t xA = (x >> 1) ^ ((x & 1) * MATRIX_A);
        uint32_t val = table[i + M] ^ xA;

        // テンパリング処理の最適化
        val ^= val >> 11;
        val ^= (val << 7) & 0x9D2C5680;
        val ^= (val << 15) & 0xEFC60000;
        val ^= val >> 18;
        val >>= 27;

        ivsCode |= val << shift;
    }

    return ivsCode;
}

uint32_t MT_0(uint64_t seed0, uint32_t p) {
    uint64_t seed1 = seed0 * 0x5D588B656C078965UL + 0x269EC3UL; // 64ビットシードを使用してseed1を生成
    // cout << hex << seed1 << endl; // seed1の値を表示
    return MT_1(seed1, p);
}

int main() {
    uint64_t seedi = 0xADFA217848890B0D; // 例: 任意の64ビットシード
    cout << hex << MT_1(seedi, 0) << endl; // 例: pを0に設定して関数を呼び出す この時の初期seedはプラスルツールのseed1に該当
    uint64_t seedii = 0xDFA564DA62B08D7A; // 例: 任意の64ビットシード
    cout << hex << MT_1(seedii, 10) << endl; // 例: pを10に設定して関数を呼び出す
    uint64_t seediii = 0x7DB401AF143D1B82; // 例: 任意の64ビットシード
    cout << hex << MT_0(seediii, 0) << endl; // 例: MT_0関数を呼び出す
    return 0;
}

// todo:ハードウェアアクセラレーションについて勉強して書き換える