#include <iostream>
#include <array>

using namespace std;

void MT_0(uint64_t seed0) {
    array<uint32_t, 403> MT;
    MT[0] = static_cast<uint32_t>(seed0 >> 32); // seed0の上位32ビットをMT[0]に設定
    for(int i = 1; i < 403; ++i) {
        MT[i] = (1812433253 * (MT[i - 1] ^ (MT[i - 1] >> 30)) + i);
    }

    // return 0; // 必要に応じて適切な戻り値を設定
}

int main() {
    uint64_t seed = 0x123456789ABCDEF0; // 例: 任意の64ビットシード
    MT_0(seed);
}