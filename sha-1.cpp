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
bool SHA1(SHA1_DATA* sha1d, const string& data);
bool SHA1_5(SHA1_DATA* sha1d, const Version ver, const int16_t Timer0, const bool isDSLite, const uint64_t MAC,
            const tm& timeInfo);
bool SHA1_5(SHA1_DATA* sha1d, const Version& ver);
bool SHA1_Array(SHA1_DATA* sha1d, const array<uint32_t, 16>& data);
bool SHA1_2(SHA1_DATA* sha1d, const Version& ver, const uint16_t Timer0, const bool isDSLite, const uint64_t MAC,
            const GameDate& gameDate);

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

bool SHA1_2(SHA1_DATA* sha1d, const Version& ver, const uint16_t Timer0, const bool isDSLite, const uint64_t MAC, const GameDate& gameDate) {
    if (!sha1d) return false;

    // nazo arrayを取得
    const auto& nazoArray = ver.getNazoArray();

    // 16個の32ビット値を格納するデータ配列
    array<uint32_t, 16> data = {};

    // nazo arrayをdataにコピー
    for (size_t i = 0; i < nazoArray.size(); ++i) {
        data[i] = SHA_Reverse(nazoArray[i]);
    }

    // VCountとTimer0を結合してdata[5]に格納
    uint32_t VCount = ver.getVCount();
    data[5] = SHA_Reverse((VCount << 16) | Timer0);

    // MACの下位16ビットをdata[6]に格納
    data[6] = (MAC & 0xFFFF);

    // MACの真ん中32ビットとGxFrameXorFrameのXOR結果をdata[7]に格納
    uint32_t middleMAC = (MAC >> 16) & 0xFFFFFFFF;
    uint32_t GxFrameXorFrame = isDSLite ? 0x6000006 : 0x8000006;
    data[7] = middleMAC ^ GxFrameXorFrame;

    // 日付・曜日をdata[8]に格納
    data[8] = gameDate.getDate8Format();

    // 時刻をdata[9]に格納
    data[9] = gameDate.getTime9Format();

    // 固定値
    data[12] = 0xFF2F0000;
	data[13] = 0x80000000;
	data[15] = 0x000001A0;

    // SHA1_Array関数を呼び出してハッシュ計算
    return SHA1_Array(sha1d, data);
}

bool SHA1_Array(SHA1_DATA* sha1d, const array<uint32_t, 16>& data) {
    if (!sha1d) return false;

    // デバッグ出力：データ配列の内容を16進数で表示
    cout << "\nDebug: Data array content before hash calculation:" << endl;
    for (size_t i = 0; i < data.size(); ++i) {
        cout << "data[" << dec << i << "]: 0x" << hex << setfill('0') << setw(8) << data[i] << endl;
    }
    cout << "------------------------" << endl;

    array<SHA_INT_TYPE, 5> hashValues = SHA1_H_Val;

    // データを512ビット（64バイト）単位で処理
    unsigned char buffer[64] = {};
    for (size_t i = 0; i < data.size(); ++i) {
        memcpy(buffer + (i * 4), &data[i], sizeof(uint32_t));
    }

    // SHA1 ハッシュ計算
    SHA1_HashBlock(hashValues, buffer);

    // ハッシュ値を出力
    memcpy(sha1d->Value, hashValues.data(), sizeof(hashValues));
    snprintf(sha1d->Val_String, sizeof(sha1d->Val_String), "%08X %08X %08X %08X %08X",
             hashValues[0], hashValues[1], hashValues[2], hashValues[3], hashValues[4]);

    return true;
}

// テスト用メイン関数
int main() {
    SHA1_DATA sha1d;

    try {
        string label;
        cout << "Enter version label (e.g., JPB1, JPW1, JPB2, JPW2): ";
        cin >> label;

        // Versionオブジェクトを作成
        Version version(label);

        uint16_t Timer0;
        cout << "Enter Timer0 value (hex, e.g., 0x0C7A): ";
        cin >> hex >> Timer0;

        bool isDSLite;
        cout << "Is it a DS Lite? (1 for true, 0 for false): ";
        cin >> isDSLite;

        uint64_t MAC;
        cout << "Enter MAC address (hex, e.g., 0x0009BF6D93CE): ";
        cin >> hex >> MAC;

        // uint8_t year, month, day, hour, minute, second;
        // cout << "Enter year (last two digits, e.g., 60 for 2060): ";
        // cin >> dec >> year;
		// cout << "Entered year: " << year << endl;
		// if (year > 99) {
		// 	throw invalid_argument("Year must be under 100 (last two digits).");
		// 	return 1;
		// }
        // cout << "Enter month (1-12): ";
        // cin >> month;
		// cout << "Entered month: " << dec << month << endl;
        // cout << "Enter day (1-31): ";
        // cin >> day;
        // cout << "Enter hour (0-23): ";
        // cin >> hour;
        // cout << "Enter minute (0-59): ";
        // cin >> minute;
        // cout << "Enter second (0-59): ";
        // cin >> second;

        // GameDateオブジェクトを作成
        // GameDate gameDate(year, month, day, hour, minute, second);
		GameDate gameDate(60, 3, 23, 13, 1, 18); // 2060年2月23日13時01分18秒
		// GameDateオブジェクトを作成（例: 2060年2月23日13時01分18秒）

        // 日付と時刻を出力
        cout << "You entered date and time: ";
        gameDate.print();

        // SHA1_2関数を呼び出してハッシュを計算
        if (SHA1_2(&sha1d, version, Timer0, isDSLite, MAC, gameDate)) {
            cout << "SHA1 Hash: " << sha1d.Val_String << endl;

            // // デバッグ情報の表示
            // version.print();  // nazoArrayとVCountの値を表示
            // cout << "Timer0: 0x" << hex << Timer0 << endl;
            // cout << "Combined VCount and Timer0 (data[5]): 0x" << hex << ((version.getVCount() << 16) | Timer0) << endl;
            // cout << "Value for data[6]: 0x" << hex << (MAC & 0xFFFF) << endl;
            // cout << "Value for data[7]: 0x" << hex << (((MAC >> 16) & 0xFFFFFFFF) ^ (isDSLite ? 0x6000006 : 0x6000008)) << endl;
        } else {
            cout << "Error: SHA1_2 computation failed." << endl;
        }

    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}