#include "sha-1.h"
#include <cstring>
#include <iostream>
#include <cstdio>
#include <array>
#include <string>
#include <algorithm>

using namespace std;

// 型定義
using SHA_INT_TYPE = uint32_t;

// SHA-1の初期ハッシュ値
constexpr array<SHA_INT_TYPE, 5> SHA1_H_Val = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};

// プロトタイプ宣言
SHA_INT_TYPE SHA1_K(SHA_INT_TYPE t);
SHA_INT_TYPE SHA1_f(SHA_INT_TYPE t, SHA_INT_TYPE B, SHA_INT_TYPE C, SHA_INT_TYPE D);
SHA_INT_TYPE SHA1_rotl(SHA_INT_TYPE r, SHA_INT_TYPE x);
void SHA1_HashBlock(array<SHA_INT_TYPE, 5>& hashValues, const unsigned char* data);
void SHA_Reverse_INT64(unsigned char* data, uint64_t value);
SHA_INT_TYPE SHA_Reverse(SHA_INT_TYPE value);
bool SHA1(SHA1_DATA* sha1d, const string& data);

// 定数取得関数
inline SHA_INT_TYPE SHA1_K(SHA_INT_TYPE t) {
    if (t <= 19) return 0x5a827999;
    if (t <= 39) return 0x6ed9eba1;
    if (t <= 59) return 0x8f1bbcdc;
    return 0xca62c1d6;
}

// 論理関数
inline SHA_INT_TYPE SHA1_f(SHA_INT_TYPE t, SHA_INT_TYPE B, SHA_INT_TYPE C, SHA_INT_TYPE D) {
    if (t <= 19) return (B & C) | (~B & D);
    if (t <= 39) return B ^ C ^ D;
    if (t <= 59) return (B & C) | (B & D) | (C & D);
    return B ^ C ^ D;
}

// 左ローテート関数
inline SHA_INT_TYPE SHA1_rotl(SHA_INT_TYPE r, SHA_INT_TYPE x) {
    return (x << r) | (x >> (32 - r));
}

// 32ビット値のエンディアン変換
inline SHA_INT_TYPE SHA_Reverse(SHA_INT_TYPE value) {
    return ((value & 0xFF) << 24) |
           ((value & 0xFF00) << 8) |
           ((value & 0xFF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

// 64ビット値のエンディアン変換
void SHA_Reverse_INT64(unsigned char* data, uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        data[i] = (value >> (56 - i * 8)) & 0xFF;
    }
}

// 512ビットブロックのハッシュ計算
void SHA1_HashBlock(array<SHA_INT_TYPE, 5>& hashValues, const unsigned char* data) {
    array<SHA_INT_TYPE, 80> words = {};
    for (int i = 0; i < 16; ++i) {
        words[i] = SHA_Reverse(reinterpret_cast<const uint32_t*>(data)[i]);
    }
    for (int t = 16; t < 80; ++t) {
        words[t] = SHA1_rotl(1, words[t - 3] ^ words[t - 8] ^ words[t - 14] ^ words[t - 16]);
    }

    // 初期値をコピー
    SHA_INT_TYPE a = hashValues[0];
    SHA_INT_TYPE b = hashValues[1];
    SHA_INT_TYPE c = hashValues[2];
    SHA_INT_TYPE d = hashValues[3];
    SHA_INT_TYPE e = hashValues[4];

    // 80ラウンドの計算
    for (int t = 0; t < 80; ++t) {
        SHA_INT_TYPE temp = SHA1_rotl(5, a) + SHA1_f(t, b, c, d) + e + words[t] + SHA1_K(t);
        e = d;
        d = c;
        c = SHA1_rotl(30, b);
        b = a;
        a = temp;
    }

    // ハッシュ値を更新
    hashValues[0] += a;
    hashValues[1] += b;
    hashValues[2] += c;
    hashValues[3] += d;
    hashValues[4] += e;
}

// SHA-1アルゴリズム
bool SHA1(SHA1_DATA* sha1d, const string& data) {
    if (!sha1d) return false;

    array<SHA_INT_TYPE, 5> hashValues = SHA1_H_Val;
    size_t dataSize = data.size();
    size_t fullBlocks = dataSize / 64;
    size_t remainingBytes = dataSize % 64;

    // 64バイトごとに処理
    for (size_t i = 0; i < fullBlocks; ++i) {
        SHA1_HashBlock(hashValues, reinterpret_cast<const unsigned char*>(data.data() + i * 64));
    }

    // パディング処理
    unsigned char buffer[64] = {};
    memcpy(buffer, data.data() + fullBlocks * 64, remainingBytes);
    buffer[remainingBytes] = 0x80;

    if (remainingBytes >= 56) {
        SHA1_HashBlock(hashValues, buffer);
        memset(buffer, 0, 64);
    }

    uint64_t bitLength = dataSize * 8;
    SHA_Reverse_INT64(buffer + 56, bitLength);
    SHA1_HashBlock(hashValues, buffer);

    // ハッシュ値を出力
    memcpy(sha1d->Value, hashValues.data(), sizeof(hashValues));
    sprintf(sha1d->Val_String, "%08X %08X %08X %08X %08X",
            hashValues[0], hashValues[1], hashValues[2], hashValues[3], hashValues[4]);
    return true;
}

// テスト用メイン関数
int main() {
    SHA1_DATA sha1d;
    string input;

    cout << "Enter data (max 2048 bytes): ";
    getline(cin, input);

    if (input.size() > 2048) {
        cerr << "Error: Input exceeds 2048 bytes." << endl;
        return 1;
    }

    if (SHA1(&sha1d, input)) {
        cout << "SHA1: " << sha1d.Val_String << endl;
    } else {
        cerr << "Error: SHA1 computation failed." << endl;
    }

    return 0;
}