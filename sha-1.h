#ifndef SHA1_H
#define SHA1_H

#include <string>
#include <cstdint>

struct SHA1_DATA {
    uint32_t Value[5];     // 従来のハッシュ値配列
    uint64_t Iseed;   // 上位16バイトを64ビット値として保持
    char Val_String[50];   // 文字列表現
};

// SHA1関数の宣言
bool SHA1(SHA1_DATA* sha1d, const char* data, unsigned int size);

#endif // SHA1_H