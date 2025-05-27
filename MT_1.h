#ifndef MT_1_H
#define MT_1_H

#include <cstdint>

// メルセンヌツイスタのパラメータ
constexpr int N = 624;
constexpr int M = 397;
constexpr uint32_t UPPER_MASK = 0x80000000;
constexpr uint32_t LOWER_MASK = 0x7fffffff;
constexpr uint32_t MATRIX_A = 0x9908B0DF;

// テンパリング用の定数
constexpr uint32_t TEMPERING_MASK_B = 0x9D2C5680;
constexpr uint32_t TEMPERING_MASK_C = 0xEFC60000;
constexpr uint32_t TEMPERING_SHIFT_U = 11;
constexpr uint32_t TEMPERING_SHIFT_S = 7;
constexpr uint32_t TEMPERING_SHIFT_T = 15;
constexpr uint32_t TEMPERING_SHIFT_L = 18;
constexpr uint32_t FINAL_RIGHT_SHIFT = 27;

// 関数プロトタイプ
uint32_t MT_1(uint64_t seed1, uint32_t p);

#endif // MT_0_H