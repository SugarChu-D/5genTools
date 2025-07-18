#include "sha-1.h"
#include "const.cpp"
#include "GameDate.cpp"
#include <iostream>
#include <iomanip>
#include <array>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <bitset>

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
uint64_t SHA_Reverse_INT64(uint64_t value);
bool SHA1(SHA1_DATA* sha1d, const string& data);
bool SHA1_5(SHA1_DATA* sha1d, const Version ver, const int16_t Timer0, const bool isDSLite, const uint64_t MAC,
            const tm& timeInfo);
bool SHA1_5(SHA1_DATA* sha1d, const Version& ver);
uint64_t SHA1_Array(SHA1_DATA* sha1d, const array<uint32_t, 16>& data);

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

// 64ビット値のエンディアン変換（新しい関数）
inline uint64_t SHA_Reverse_INT64(uint64_t value) {
    return ((value & 0x00000000000000FFULL) << 56) |
           ((value & 0x000000000000FF00ULL) << 40) |
           ((value & 0x0000000000FF0000ULL) << 24) |
           ((value & 0x00000000FF000000ULL) << 8) |
           ((value & 0x000000FF00000000ULL) >> 8) |
           ((value & 0x0000FF0000000000ULL) >> 24) |
           ((value & 0x00FF000000000000ULL) >> 40) |
           ((value & 0xFF00000000000000ULL) >> 56);
}

// 512ビットブロックのハッシュ計算
void SHA1_HashBlock(array<SHA_INT_TYPE, 5>& hashValues, const unsigned char* data) {
    array<SHA_INT_TYPE, 80> words = {};
    for (int i = 0; i < 16; ++i) {
        words[i] = reinterpret_cast<const uint32_t*>(data)[i]; // 本来はエンディアン変換をするが、ここでは省略
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
// bool SHA1(SHA1_DATA* sha1d, const string& data) {
//     if (!sha1d) return false;

//     array<SHA_INT_TYPE, 5> hashValues = SHA1_H_Val;
//     size_t dataSize = data.size();
//     size_t fullBlocks = dataSize / 64;
//     size_t remainingBytes = dataSize % 64;

//     // 64バイトごとに処理
//     for (size_t i = 0; i < fullBlocks; ++i) {
//         SHA1_HashBlock(hashValues, reinterpret_cast<const unsigned char*>(data.data() + i * 64));
//     }

//     // パディング処理
//     unsigned char buffer[64] = {};
//     memcpy(buffer, data.data() + fullBlocks * 64, remainingBytes);
//     buffer[remainingBytes] = 0x80;

//     if (remainingBytes >= 56) {
//         SHA1_HashBlock(hashValues, buffer);
//         memset(buffer, 0, 64);
//     }

//     uint64_t bitLength = dataSize * 8;
//     SHA_Reverse_INT64(buffer + 56, bitLength);
//     SHA1_HashBlock(hashValues, buffer);

//     // ハッシュ値を出力
//     memcpy(sha1d->Value, hashValues.data(), sizeof(hashValues));
//     sprintf(sha1d->Val_String, "%08X %08X %08X %08X %08X",
//             hashValues[0], hashValues[1], hashValues[2], hashValues[3], hashValues[4]);
//     return true;
// }

uint64_t initialSEED(SHA1_DATA* sha1d, const InitialSeedParams& params, const GameDate& gameDate, const uint16_t keypress) {
    if (!sha1d) return 0;

    // nazo arrayを取得
    const auto& nazoArray = params.ver.getNazoArray();

    // 16個の32ビット値を格納するデータ配列
    array<uint32_t, 16> data = {};

    // nazo arrayをdataにコピー
    for (size_t i = 0; i < nazoArray.size(); ++i) {
        data[i] = SHA_Reverse(nazoArray[i]);
    }

    // VCountとTimer0を結合してdata[5]に格納
    uint32_t VCount = params.ver.getVCount();
    data[5] = SHA_Reverse((VCount << 16) | params.Timer0);

    // MACの下位16ビットをdata[6]に格納
    data[6] = (params.MAC & 0xFFFF);

    // MACの真ん中32ビットとGxFrameXorFrameのXOR結果をdata[7]に格納
    uint32_t middleMAC = (params.MAC >> 16) & 0xFFFFFFFF;
    uint32_t GxFrameXorFrame = params.isDSLite ? 0x6000006 : 0x8000006;
    data[7] = middleMAC ^ GxFrameXorFrame;

    // 日付・曜日をdata[8]に格納
    data[8] = gameDate.getDate8Format();

    // 時刻をdata[9]に格納
    data[9] = gameDate.getTime9Format();

    // 固定値
    data[12] = SHA_Reverse(keypress);
	data[13] = 0x80000000;
	data[15] = 0x000001A0;

    // SHA1_Array関数を呼び出してハッシュ計算してIseedを返す
    return SHA1_Array(sha1d, data);
}

uint64_t SHA1_Array(SHA1_DATA* sha1d, const array<uint32_t, 16>& data) {
    if (!sha1d) return 0;

    array<SHA_INT_TYPE, 5> hashValues = SHA1_H_Val;

    // SHA1 ハッシュ計算を直接実行
    SHA1_HashBlock(hashValues, reinterpret_cast<const unsigned char*>(data.data()));

    // ハッシュ値を出力
    memcpy(sha1d->Value, hashValues.data(), sizeof(hashValues));

    // 上位16バイトをuint64_tとして保持
    uint64_t Iseed = (static_cast<uint64_t>(hashValues[0]) << 32) | hashValues[1];
    sha1d->Iseed = Iseed;

    // エンディアン変換したIseedを返す
    return SHA_Reverse_INT64(Iseed);
}

// テスト用メイン関数
// int main() {
//     SHA1_DATA sha1d;

//     try {
//         // 固定パラメータの設定
//         Version version("JPW1");
//         uint16_t Timer0 = 0x0C69;
//         bool isDSLite = false;
//         uint64_t MAC = 0x0009BF6D93CE;
//         GameDate gameDate(60, 3, 23, 13, 1, 18);

//         // パラメータの構造体を作成
//         InitialSeedParams params = {
//             version,
//             Timer0,
//             isDSLite,
//             MAC
//         };

//         cout << "パラメータ設定:" << endl;
//         cout << "Version: JPB1" << endl;
//         cout << "Timer0: 0x" << hex << Timer0 << endl;
//         cout << "DS Lite: " << (isDSLite ? "Yes" : "No") << endl;
//         cout << "MAC: 0x" << hex << MAC << endl;
//         cout << "Date/Time: ";
//         gameDate.print();
//         cout << endl;

//         // テスト用の探索（0x2ff0から0x3000までの範囲）
//         cout << "Keypress探索開始..." << endl;
//         for (uint16_t keypress = 0x2ff0; keypress <= 0x3000; ++keypress) {
//             uint64_t seed = initialSEED(&sha1d, params, gameDate, keypress);
//             cout << "Keypress: 0x" << hex << setw(4) << setfill('0') << keypress 
//                  << " -> Seed: 0x" << hex << setw(16) << setfill('0') << seed << endl;
//         }

//     } catch (const exception& e) {
//         cerr << "エラー: " << e.what() << endl;
//         return 1;
//     }

//     return 0;
// }