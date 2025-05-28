#include "MT_1.h"
#include <chrono>
#include <iostream>
#include <stdexcept>

using namespace std;

uint32_t MT_1(uint64_t seed1, uint32_t p) {
    constexpr uint32_t MAX_P = 16;
    constexpr uint32_t TABLE_SIZE = MAX_P + 6 + M;
    uint32_t table[TABLE_SIZE];
    table[0] = static_cast<uint32_t>(seed1 >> 32);

    // 初期化ループ
    uint32_t prev = table[0];
    for(uint32_t i = 1; i < p + 6 + M; ++i) {
        prev = table[i] = 1812433253U * (prev ^ (prev >> 30)) + i;
    }

    uint32_t ivsCode = 0;

    // メインループ
    for(int i = p, shift = 25; i < p + 6; ++i, shift -= 5) {  // shift を 25 から開始して 5 ずつ減少
        uint32_t x = (table[i] & UPPER_MASK) | (table[i + 1] & LOWER_MASK);
        uint32_t xA = (x >> 1) ^ ((x & 1) * MATRIX_A);
        uint32_t val = table[i + M] ^ xA;

        // テンパリング処理
        val ^= val >> 11;
        val ^= (val << 7) & 0x9D2C5680;
        val ^= (val << 15) & 0xEFC60000;
        val ^= val >> 18;
        val >>= 27;

        ivsCode |= val << shift;  // 値を逆順で格納
    }

    return ivsCode;
}

uint32_t MT_0(uint64_t seed0, uint32_t p) {
    uint64_t seed1 = seed0 * 0x5D588B656C078965UL + 0x269EC3UL; // LCGでseed1を生成
    // cout << hex << seed1 << endl; // seed1の値を表示
    return MT_1(seed1, p);
}

uint32_t MT_32(uint32_t seed, uint32_t p) {    
    // pの最大値が16なら、tableのサイズは最大で16+6+397=419
    constexpr uint32_t MAX_P = 16;
    constexpr uint32_t TABLE_SIZE = MAX_P + 6 + M;
    uint32_t table[TABLE_SIZE];
    table[0] = seed;

    // 初期化ループ
    uint32_t prev = table[0];
    for(uint32_t i = 1; i < p + 6 + M; ++i) {
        prev = table[i] = 1812433253U * (prev ^ (prev >> 30)) + i;
    }

    uint32_t ivsCode = 0;

    // メインループ
    for(int i = p, shift = 25; i < p + 6; ++i, shift -= 5) {  // shift を 25 から開始して 5 ずつ減少
        uint32_t x = (table[i] & UPPER_MASK) | (table[i + 1] & LOWER_MASK);
        uint32_t xA = (x >> 1) ^ ((x & 1) * MATRIX_A);
        uint32_t val = table[i + M] ^ xA;

        // テンパリング処理
        val ^= val >> 11;
        val ^= (val << 7) & 0x9D2C5680;
        val ^= (val << 15) & 0xEFC60000;
        val ^= val >> 18;
        val >>= 27;

        ivsCode |= val << shift;  // 値を逆順で格納
    }

    return ivsCode;
}

// int main() {
//     uint32_t seed = 0x419534ab;
//     uint32_t p = 10;

//     auto start = std::chrono::high_resolution_clock::now();
//     uint32_t result = MT_32(seed, p);
//     auto end = std::chrono::high_resolution_clock::now();

//     auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//     std::cout << "MT_32 result: " << std::hex << result << std::endl;
//     std::cout << "MT_32 計算時間: " << duration_ns << " ns" << std::endl;

//     return 0;
// }

// todo:ハードウェアアクセラレーションについて勉強して書き換える