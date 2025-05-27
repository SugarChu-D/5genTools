#include <iostream>
#include <cstdint>
#include <vector>

using namespace std;

constexpr int N = 624;
constexpr int M = 397;
// Mersenne Twisterの初期化関数

uint32_t MT(uint64_t seed0, uint32_t p) {
    uint16_t length = p + 6 + M; // p + 6 + Mの長さを計算
    if (length < 0 || length > N) {
        throw invalid_argument("Length must be between 0 and N (624).");
    }
    if (p < 0 || p >= N) {
        throw invalid_argument("p must be between 0 and N (624).");
    }
    vector<uint32_t> table(length); // 動的配列を使用してMTを初期化
    table.at(0) = static_cast<uint32_t>(seed0 >> 32); // seed0の上位32ビットをMT[0]に設定

    for(uint32_t i = 1; i < length; ++i) {
        table[i] = (1812433253 * (table[i - 1] ^ (table[i - 1] >> 30)) + i);
    }
    // ここで、tableの要素をMersenne Twisterのアルゴリズムに従って初期化

    uint32_t ivsCode = 0;

    for(int i=p; i<p+6; ++i) {
        uint32_t x = (table[i] & 0x80000000) + (table[(i+1)] & 0x7fffffff);
        uint32_t xA = x >> 1;
        if (x & 1) xA ^= 0x9908B0DF; // 0x9908B0DFはMersenne Twisterのマスク
        uint32_t val = table[i + M] ^ xA;

        val = val ^ (val >> 11);
        val = val ^ (val << 7 & 0x9D2C5680);
        val = val ^ (val << 15 & 0xEFC60000);
        val = val ^ (val >> 18);

        val >>= 27;
        cout<< val << " ";

        ivsCode |= val << (5 *(i-p));
    }
    return ivsCode; // 必要に応じて適切な戻り値を設定
}




int main() {
    uint64_t seed1 = 0xADFA217848890B0D; // 例: 任意の64ビットシード
    cout << hex << MT(seed1, 0) << endl; // 例: pを0に設定して関数を呼び出す この時の初期seedはプラスルツールのseed1に該当
    uint64_t seed2 = 0xDFA564DA62B08D7A; // 例: 任意の64ビットシード
    cout << hex << MT(seed2, 10) << endl; // 例: pを0に設定して関数を呼び出す この時の初期seedはプラスルツールのseed2に該当

    return 0;
}